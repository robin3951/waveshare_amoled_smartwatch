// BLE peripheral that speaks the Chronos protocol (fbiego/chronos-esp32).
// Service: Nordic UART Service  6e400001-b5a3-f393-e0a9-e50e24dcca9e
//   RX char (phone → watch): 6e400002-...  WRITE / WRITE_NO_RSP
//   TX char (watch → phone): 6e400003-...  NOTIFY
//
// Notification packet from phone:
//   AB [len_hi] [len_lo] FF 72 80 [icon] 02 [text...]
// Watch responds on subscribe with an info packet and a battery packet.

#include "ble_chronos.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_random.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/ble_store.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "store/config/ble_store_config.h"
#include "ui.h"

static const char* TAG = "BLE_CHRONOS";

#define DEVICE_NAME "Chronos"

// UUIDs in little-endian byte order
// 6e400001-b5a3-f393-e0a9-e50e24dcca9e  (service)
static const ble_uuid128_t svc_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);
// 6e400002-...  RX: phone writes here
static const ble_uuid128_t rx_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);
// 6e400003-...  TX: watch notifies phone
static const ble_uuid128_t tx_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

static uint16_t tx_val_handle;
static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint8_t own_addr_type;
// Reassembly buffer for multi-packet Chronos messages
static bool s_connected = false;

static uint8_t s_pkt_buf[512];
static uint16_t s_pkt_len = 0;  // bytes collected so far
static uint16_t s_pkt_total =
    0;  // total bytes expected (3 header + payload length)

/* ── App icon ID → display name ──────────────────────────────────────── */

static const char* icon_to_name(uint8_t icon) {
  switch (icon) {
    case 0x01:
      return "Anruf";
    case 0x02:
      return "Entgangen";
    case 0x03:
      return "SMS";
    case 0x09:
      return "WeChat";
    case 0x0A:
      return "WhatsApp";
    case 0x0B:
      return "E-Mail";
    case 0x0C:
      return "Kalender";
    case 0x0D:
      return "Google";
    case 0x0E:
      return "Twitter";
    case 0x0F:
      return "Facebook";
    case 0x11:
      return "LinkedIn";
    case 0x12:
      return "Instagram";
    case 0x13:
      return "Viber";
    case 0x14:
      return "Skype";
    case 0x16:
      return "Line";
    case 0x17:
      return "Telegram";
    case 0x20:
      return "Snapchat";
    default:
      return "Benachricht.";
  }
}

/* ── Packet helpers ──────────────────────────────────────────────────── */

static void notify_tx(uint16_t ch, const uint8_t* data, uint16_t len) {
  struct os_mbuf* om = ble_hs_mbuf_from_flat(data, len);
  if (!om) return;
  int rc = ble_gatts_notify_custom(ch, tx_val_handle, om);
  if (rc) ESP_LOGW(TAG, "notify rc=%d", rc);
}

// Info packet: identifies this watch to the Chronos app
static void send_info(uint16_t ch) {
  static const uint8_t pkt[] = {
      0xAB, 0x00, 0x11, 0xFF, 0x92, 0xC0, 0x01, 0x00,  // firmware v1.0
      0x00, 0xFB, 0x1E, 0x40, 0xC0, 0x0E, 0x32, 0x28, 0x00, 0xE2,
      0x02,  // screen config byte
      0x80};
  notify_tx(ch, pkt, sizeof(pkt));
}

// Battery status packet
static void send_battery(uint16_t ch, uint8_t pct, bool charging) {
  uint8_t pkt[] = {
      0xAB, 0x00, 0x05, 0xFF, 0x91, 0x80, (uint8_t)(charging ? 0x01 : 0x00),
      pct};
  notify_tx(ch, pkt, sizeof(pkt));
}

/* ── Incoming packet parser ──────────────────────────────────────────── */

static void parse(const uint8_t* d, uint16_t len) {
  if (len < 6 || d[0] != 0xAB || d[3] != 0xFF) return;

  uint8_t c0 = d[4], c1 = d[5];

  // Notification: FF 72 80 [icon] [state=02] [text...]
  if (c0 == 0x72 && c1 == 0x80 && len >= 9) {
    if (d[7] != 0x02) return;  // state must be "new"

    uint16_t tlen = len - 8;
    char raw[256];
    if (tlen >= sizeof(raw)) tlen = (uint16_t)(sizeof(raw) - 1);
    memcpy(raw, d + 8, tlen);
    raw[tlen] = '\0';

    const char* icon_name = icon_to_name(d[6]);

    // Split "Sender: Message" or "Sender\nMessage" into title + body.
    char title[64];  // "[WhatsApp] John Doe"
    char body[256];
    char* sep = strchr(raw, '\n');
    if (!sep) sep = strstr(raw, ": ");
    if (sep && sep != raw) {
      // Sender name found before separator
      size_t slen = (size_t)(sep - raw);
      char sender[48];
      if (slen >= sizeof(sender)) slen = sizeof(sender) - 1;
      memcpy(sender, raw, slen);
      sender[slen] = '\0';
      snprintf(title, sizeof(title), "[%s] %s", icon_name, sender);
      const char* rest = sep + (*sep == '\n' ? 1 : 2);
      snprintf(body, sizeof(body), "%s", rest);
    } else {
      // No sender split possible — just show app name as title
      snprintf(title, sizeof(title), "%s", icon_name);
      snprintf(body, sizeof(body), "%s", raw);
    }

    ESP_LOGI(TAG, "Notif raw='%.120s' → title='%s' body='%.80s'", raw, title,
             body);

    lvgl_port_lock(0);
    ui_add_notification(title, body);
    lvgl_port_unlock();

    // Device info / version request from app
  } else if ((c0 == 0x92 && c1 == 0xC0) || c0 == 0xCA) {
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) send_info(conn_handle);
  }
}

/* ── GATT callbacks ──────────────────────────────────────────────────── */

static int rx_cb(uint16_t ch, uint16_t ah, struct ble_gatt_access_ctxt* ctxt,
                 void* arg) {
  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) return 0;

  uint16_t chunk_len = OS_MBUF_PKTLEN(ctxt->om);
  if (chunk_len == 0) return 0;

  uint8_t chunk[512];
  if (chunk_len > sizeof(chunk)) chunk_len = (uint16_t)sizeof(chunk);
  os_mbuf_copydata(ctxt->om, 0, chunk_len, chunk);

  if (chunk_len >= 3 && chunk[0] == 0xAB) {
    // First packet — extract declared total length and start fresh
    s_pkt_len = 0;
    s_pkt_total = 3u + (((uint16_t)chunk[1] << 8) | chunk[2]);
    if (s_pkt_total > sizeof(s_pkt_buf))
      s_pkt_total = (uint16_t)sizeof(s_pkt_buf);
    ESP_LOGD(TAG, "RX start: declared=%d chunk=%d", s_pkt_total, chunk_len);
  } else if (s_pkt_total == 0) {
    return 0;  // continuation without a prior header — discard
  }

  // Append chunk into reassembly buffer
  uint16_t space = s_pkt_total - s_pkt_len;
  if (chunk_len > space) chunk_len = space;
  memcpy(s_pkt_buf + s_pkt_len, chunk, chunk_len);
  s_pkt_len += chunk_len;

  if (s_pkt_len >= s_pkt_total) {
    // Complete message assembled — parse and reset
    ESP_LOGD(TAG, "RX complete: %d bytes", s_pkt_len);
    parse(s_pkt_buf, s_pkt_len);
    s_pkt_len = 0;
    s_pkt_total = 0;
  }
  return 0;
}

static int tx_cb(uint16_t ch, uint16_t ah, struct ble_gatt_access_ctxt* ctxt,
                 void* arg) {
  return 0;  // TX is notify-only; reads return empty
}

/* ── GATT service table ──────────────────────────────────────────────── */

static const struct ble_gatt_svc_def svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    // RX char — phone writes notifications here
                    .uuid = &rx_uuid.u,
                    .access_cb = rx_cb,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                },
                {
                    // TX char — watch notifies phone (no READ; empty read
                    // causes ATT error)
                    .uuid = &tx_uuid.u,
                    .access_cb = tx_cb,
                    .flags = BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &tx_val_handle,
                },
                {0}},
    },
    {0}};

/* ── GAP / advertising ───────────────────────────────────────────────── */

static void start_adv(void);

static int gap_cb(struct ble_gap_event* ev, void* arg) {
  switch (ev->type) {
    case BLE_GAP_EVENT_CONNECT:
      if (ev->connect.status == 0) {
        conn_handle = ev->connect.conn_handle;
        s_connected = true;
        ESP_LOGI(TAG, "Connected handle=%d", conn_handle);
        // Request larger MTU so phone can send up to 509 bytes per packet
        ble_gattc_exchange_mtu(conn_handle, NULL, NULL);
        lvgl_port_lock(0);
        ui_set_ble_status(true);
        lvgl_port_unlock();
      } else {
        start_adv();
      }
      break;

    case BLE_GAP_EVENT_DISCONNECT:
      ESP_LOGI(TAG, "Disconnected reason=%d", ev->disconnect.reason);
      conn_handle = BLE_HS_CONN_HANDLE_NONE;
      s_connected = false;
      lvgl_port_lock(0);
      ui_set_ble_status(false);
      lvgl_port_unlock();
      start_adv();
      break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
      start_adv();
      break;

    // When phone enables notifications on TX characteristic: send handshake
    case BLE_GAP_EVENT_SUBSCRIBE:
      if (ev->subscribe.attr_handle == tx_val_handle &&
          ev->subscribe.cur_notify) {
        ESP_LOGI(TAG, "TX subscribed, sending handshake");
        send_info(ev->subscribe.conn_handle);
        send_battery(ev->subscribe.conn_handle, 80, false);
      }
      break;

    case BLE_GAP_EVENT_MTU:
      ESP_LOGI(TAG, "MTU=%d", ev->mtu.value);
      break;

    // Android has cached an old LTK that we no longer have → delete and retry
    case BLE_GAP_EVENT_REPEAT_PAIRING: {
      struct ble_gap_conn_desc desc;
      if (ble_gap_conn_find(ev->repeat_pairing.conn_handle, &desc) == 0) {
        ble_store_util_delete_peer(&desc.peer_id_addr);
      }
      return BLE_GAP_REPEAT_PAIRING_RETRY;
    }

    case BLE_GAP_EVENT_ENC_CHANGE:
      ESP_LOGI(TAG, "Encryption status=%d", ev->enc_change.status);
      break;

    // With NO_IO this should never fire; if it does, accept to avoid stalling
    case BLE_GAP_EVENT_PASSKEY_ACTION: {
      struct ble_sm_io pkey = {0};
      pkey.action = ev->passkey.params.action;
      if (ev->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
        pkey.numcmp_accept = 1;
      }
      ble_sm_inject_io(ev->passkey.conn_handle, &pkey);
      break;
    }

    default:
      ESP_LOGD(TAG, "GAP event type=%d", ev->type);
      break;
  }
  return 0;
}

static void start_adv(void) {
  // Primary advertisement: flags + name + NUS service UUID (all fits in 31
  // bytes)
  struct ble_hs_adv_fields f = {0};
  f.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  f.name = (const uint8_t*)DEVICE_NAME;
  f.name_len = sizeof(DEVICE_NAME) - 1;
  f.name_is_complete = 1;
  f.uuids128 = &svc_uuid;
  f.num_uuids128 = 1;
  f.uuids128_is_complete = 1;

  int rc = ble_gap_adv_set_fields(&f);
  if (rc) {
    ESP_LOGE(TAG, "adv_fields rc=%d", rc);
    return;
  }

  struct ble_gap_adv_params p = {0};
  p.conn_mode = BLE_GAP_CONN_MODE_UND;
  p.disc_mode = BLE_GAP_DISC_MODE_GEN;
  p.itvl_min = BLE_GAP_ADV_ITVL_MS(100);
  p.itvl_max = BLE_GAP_ADV_ITVL_MS(200);

  rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &p, gap_cb, NULL);
  if (rc)
    ESP_LOGE(TAG, "adv_start rc=%d", rc);
  else
    ESP_LOGI(TAG, "Advertising as \"%s\"", DEVICE_NAME);
}

/* ── NimBLE host callbacks ───────────────────────────────────────────── */

static void on_sync(void) {
  ble_att_set_preferred_mtu(512);

  // Random static address each boot — Android won't try a cached LTK reconnect
  uint8_t rnd[6];
  esp_fill_random(rnd, sizeof(rnd));
  rnd[5] |= 0xC0;  // top 2 bits must be 11 for random static address
  int rc = ble_hs_id_set_rnd(rnd);
  if (rc) {
    ESP_LOGE(TAG, "set_rnd rc=%d", rc);
    return;
  }

  own_addr_type = BLE_OWN_ADDR_RANDOM;
  start_adv();
}

static void on_reset(int reason) {
  ESP_LOGE(TAG, "BLE host reset reason=%d", reason);
}

static void nimble_task(void* arg) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

/* ── Public API ──────────────────────────────────────────────────────── */

void ble_chronos_init(void) {
  // NimBLE requires NVS; safe to call even if already initialized
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  ret = nimble_port_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "nimble_port_init rc=%d", (int)ret);
    return;
  }

  // Wipe any stale bond/pairing data from NVS so old failed attempts don't
  // block us
  ble_store_clear();

  ble_hs_cfg.reset_cb = on_reset;
  ble_hs_cfg.sync_cb = on_sync;
  // Chronos uses plain NUS — no pairing, no bonding, no PIN
  ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
  ble_hs_cfg.sm_bonding = 0;
  ble_hs_cfg.sm_mitm = 0;
  ble_hs_cfg.sm_sc = 0;

  ble_svc_gap_init();
  ble_svc_gatt_init();

  int rc = ble_gatts_count_cfg(svcs);
  if (rc) {
    ESP_LOGE(TAG, "gatts_count_cfg rc=%d", rc);
    return;
  }
  rc = ble_gatts_add_svcs(svcs);
  if (rc) {
    ESP_LOGE(TAG, "gatts_add_svcs rc=%d", rc);
    return;
  }

  ble_svc_gap_device_name_set(DEVICE_NAME);

  nimble_port_freertos_init(nimble_task);

  ESP_LOGI(TAG, "BLE Chronos initialized, device name: %s", DEVICE_NAME);
}

bool ble_chronos_is_connected(void) { return s_connected; }
