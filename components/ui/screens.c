// UI screens — tileview-based 3-screen horizontal swipe navigation
// Display: 410 x 502 px  (width x height)
#include "screens.h"

#include <string.h>

#include "lvgl.h"

// Custom fonts with full Latin-1 (0x20-0xFF) — includes ä ö ü Ä Ö Ü ß
LV_FONT_DECLARE(lv_font_montserrat_14_ext);
LV_FONT_DECLARE(lv_font_montserrat_16_ext);

lv_obj_t* ui_tileview = NULL;
lv_obj_t* ui_tile_clock = NULL;
lv_obj_t* ui_tile_battery = NULL;
lv_obj_t* ui_tile_notif = NULL;

static lv_obj_t* dot_container = NULL;
static lv_obj_t* dots[UI_SCREEN_COUNT];

// ── Clock ────────────────────────────────────────────────────────────
static lv_obj_t* time_label = NULL;
static lv_obj_t* date_label = NULL;

// ── Battery ──────────────────────────────────────────────────────────
static lv_obj_t* bat_icon_label = NULL;
static lv_obj_t* bat_percent_label = NULL;
static lv_obj_t* bat_status_label = NULL;
static lv_obj_t* bat_voltage_label = NULL;

// ── Notification tile ────────────────────────────────────────────────
static lv_obj_t* ble_status_label = NULL;
static lv_obj_t* notif_empty_label = NULL;
static lv_obj_t* notif_scroll_cont = NULL;  // scrollable bubble container
static int notif_count = 0;

/* ─── Helpers ─────────────────────────────────────────────────────── */

static const char* get_battery_icon(int pct, bool charging) {
  if (charging) return "CHG";
  if (pct >= 90) return "100";
  if (pct >= 70) return " 75";
  if (pct >= 45) return " 50";
  if (pct >= 20) return " 25";
  return "LOW";
}

static void update_dots(int idx) {
  for (int i = 0; i < UI_SCREEN_COUNT; i++) {
    if (i == idx) {
      lv_obj_set_size(dots[i], 10, 10);
      lv_obj_set_style_bg_color(dots[i], lv_color_white(), LV_PART_MAIN);
      lv_obj_set_style_bg_opa(dots[i], LV_OPA_COVER, LV_PART_MAIN);
    } else {
      lv_obj_set_size(dots[i], 8, 8);
      lv_obj_set_style_bg_color(dots[i], lv_color_hex(0x444444), LV_PART_MAIN);
      lv_obj_set_style_bg_opa(dots[i], LV_OPA_COVER, LV_PART_MAIN);
    }
  }
}

static void tileview_changed_cb(lv_event_t* e) {
  lv_obj_t* tv = lv_event_get_target(e);
  lv_obj_t* act = lv_tileview_get_tile_active(tv);
  int idx = 0;
  if (act == ui_tile_battery)
    idx = 1;
  else if (act == ui_tile_notif)
    idx = 2;
  update_dots(idx);
}

/* ─── Tile 1: Clock ──────────────────────────────────────────────── */

static void create_clock_tile(lv_obj_t* tile) {
  lv_obj_set_style_bg_color(tile, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(tile, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tile, 0, LV_PART_MAIN);

  time_label = lv_label_create(tile);
  lv_label_set_text(time_label, "--:--:--");
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, LV_PART_MAIN);
  lv_obj_set_style_text_color(time_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -10);

  date_label = lv_label_create(tile);
  lv_label_set_text(date_label, "--.--.----");
  lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(date_label, lv_color_hex(0x888888), LV_PART_MAIN);
  lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 55);
}

/* ─── Tile 2: Battery ────────────────────────────────────────────── */

static void create_battery_tile(lv_obj_t* tile) {
  lv_obj_set_style_bg_color(tile, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(tile, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tile, 0, LV_PART_MAIN);

  bat_icon_label = lv_label_create(tile);
  lv_label_set_text(bat_icon_label, "---");
  lv_obj_set_style_text_font(bat_icon_label, &lv_font_montserrat_12,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(bat_icon_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_align(bat_icon_label, LV_ALIGN_TOP_RIGHT, -10, 10);

  bat_percent_label = lv_label_create(tile);
  lv_label_set_text(bat_percent_label, "--%");
  lv_obj_set_style_text_font(bat_percent_label, &lv_font_montserrat_48,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(bat_percent_label, lv_color_white(),
                              LV_PART_MAIN);
  lv_obj_align(bat_percent_label, LV_ALIGN_CENTER, 0, -20);

  bat_status_label = lv_label_create(tile);
  lv_label_set_text(bat_status_label, "");
  lv_obj_set_style_text_font(bat_status_label, &lv_font_montserrat_16,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(bat_status_label, lv_color_hex(0x00ff88),
                              LV_PART_MAIN);
  lv_obj_align(bat_status_label, LV_ALIGN_CENTER, 0, 45);

  bat_voltage_label = lv_label_create(tile);
  lv_label_set_text(bat_voltage_label, "-.-- V");
  lv_obj_set_style_text_font(bat_voltage_label, &lv_font_montserrat_20,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(bat_voltage_label, lv_color_hex(0x555555),
                              LV_PART_MAIN);
  lv_obj_align(bat_voltage_label, LV_ALIGN_CENTER, 0, 85);
}

/* ─── Tile 3: Notifications ──────────────────────────────────────── */

static void create_notif_tile(lv_obj_t* tile) {
  lv_obj_set_style_bg_color(tile, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(tile, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tile, 0, LV_PART_MAIN);

  // BLE status line at top (fixed, 30 px)
  ble_status_label = lv_label_create(tile);
  lv_obj_set_pos(ble_status_label, 0, 6);
  lv_obj_set_width(ble_status_label, 410);
  lv_obj_set_style_text_align(ble_status_label, LV_TEXT_ALIGN_CENTER,
                              LV_PART_MAIN);
  lv_obj_set_style_text_font(ble_status_label, &lv_font_montserrat_12,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(ble_status_label, lv_color_hex(0x555555),
                              LV_PART_MAIN);
  lv_label_set_text(ble_status_label, "BLE: Getrennt");

  // Scrollable bubble container fills the remaining height (502 - 30 = 472 px)
  notif_scroll_cont = lv_obj_create(tile);
  lv_obj_set_pos(notif_scroll_cont, 0, 30);
  lv_obj_set_size(notif_scroll_cont, 410, 456);
  lv_obj_set_style_bg_color(notif_scroll_cont, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(notif_scroll_cont, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(notif_scroll_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(notif_scroll_cont, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_row(notif_scroll_cont, 10, LV_PART_MAIN);
  lv_obj_set_scroll_dir(notif_scroll_cont, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(notif_scroll_cont, LV_SCROLLBAR_MODE_ACTIVE);
  // Flex column: children stacked vertically, centered horizontally
  lv_obj_set_flex_flow(notif_scroll_cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(notif_scroll_cont,
                        LV_FLEX_ALIGN_START,   // main axis: top-to-bottom
                        LV_FLEX_ALIGN_CENTER,  // cross axis: centered
                        LV_FLEX_ALIGN_START);

  // "No notifications" placeholder — centered, hidden once first bubble arrives
  notif_empty_label = lv_label_create(notif_scroll_cont);
  lv_obj_set_style_text_font(notif_empty_label, &lv_font_montserrat_16,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(notif_empty_label, lv_color_hex(0x444444),
                              LV_PART_MAIN);
  lv_label_set_text(notif_empty_label, "Keine\nBenachrichtigungen");
  lv_obj_set_style_text_align(notif_empty_label, LV_TEXT_ALIGN_CENTER,
                              LV_PART_MAIN);
  lv_obj_set_width(notif_empty_label, 410);
  lv_obj_align(notif_empty_label, LV_ALIGN_CENTER, 0, 0);
}

/* ─── Dot indicator overlay ──────────────────────────────────────── */

static void create_dot_indicator(lv_obj_t* screen) {
  dot_container = lv_obj_create(screen);
  lv_obj_set_style_bg_opa(dot_container, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(dot_container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(dot_container, 4, LV_PART_MAIN);
  lv_obj_set_style_pad_column(dot_container, 8, LV_PART_MAIN);
  lv_obj_set_layout(dot_container, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(dot_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(dot_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(dot_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(dot_container, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_clear_flag(dot_container,
                    LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

  for (int i = 0; i < UI_SCREEN_COUNT; i++) {
    dots[i] = lv_obj_create(dot_container);
    lv_obj_set_size(dots[i], 8, 8);
    lv_obj_set_style_radius(dots[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(dots[i], 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(dots[i], 0, LV_PART_MAIN);
    lv_obj_clear_flag(dots[i], LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
  }
  update_dots(0);
}

/* ─── Public: create all screens ────────────────────────────────── */

void create_screens(void) {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  ui_tileview = lv_tileview_create(screen);
  lv_obj_set_scrollbar_mode(ui_tileview, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_color(ui_tileview, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(ui_tileview, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_pad_all(ui_tileview, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(ui_tileview, 0, LV_PART_MAIN);

  ui_tile_clock = lv_tileview_add_tile(ui_tileview, 0, 0, LV_DIR_HOR);
  ui_tile_battery = lv_tileview_add_tile(ui_tileview, 1, 0, LV_DIR_HOR);
  ui_tile_notif = lv_tileview_add_tile(ui_tileview, 2, 0, LV_DIR_HOR);

  create_clock_tile(ui_tile_clock);
  create_battery_tile(ui_tile_battery);
  create_notif_tile(ui_tile_notif);

  create_dot_indicator(screen);

  lv_obj_add_event_cb(ui_tileview, tileview_changed_cb, LV_EVENT_VALUE_CHANGED,
                      NULL);
}

/* ─── Public: data update functions ─────────────────────────────── */

void screens_set_time(int h, int m, int s) {
  if (time_label) lv_label_set_text_fmt(time_label, "%02d:%02d:%02d", h, m, s);
}

void screens_set_date(int d, int mo, int y) {
  if (date_label) lv_label_set_text_fmt(date_label, "%02d.%02d.%04d", d, mo, y);
}

void screens_set_battery(int pct, bool charging, float voltage) {
  if (!bat_percent_label) return;

  lv_color_t color;
  if (charging)
    color = lv_color_hex(0x00ff88);
  else if (pct < 20)
    color = lv_color_hex(0xff3333);
  else if (pct < 50)
    color = lv_color_hex(0xffaa00);
  else
    color = lv_color_white();

  lv_label_set_text(bat_icon_label, get_battery_icon(pct, charging));
  lv_obj_set_style_text_color(bat_icon_label, color, LV_PART_MAIN);

  lv_label_set_text_fmt(bat_percent_label, "%d%%", pct);
  lv_obj_set_style_text_color(bat_percent_label, color, LV_PART_MAIN);

  if (charging) {
    lv_label_set_text(bat_status_label, "Laden");
    lv_obj_set_style_text_color(bat_status_label, lv_color_hex(0x00ff88),
                                LV_PART_MAIN);
  } else {
    lv_label_set_text(bat_status_label, "");
  }

  lv_label_set_text_fmt(bat_voltage_label, "%.2f V", voltage);
  lv_obj_set_style_text_color(bat_voltage_label, lv_color_hex(0x555555),
                              LV_PART_MAIN);
}

void screens_add_notification(const char* app, const char* msg) {
  if (!notif_scroll_cont) return;

  // Hide "empty" placeholder once first notification arrives
  if (notif_count == 0 && notif_empty_label) {
    lv_obj_add_flag(notif_empty_label, LV_OBJ_FLAG_HIDDEN);
  }

  // If limit reached, delete the oldest bubble (first real child after
  // placeholder)
  if (notif_count >= MAX_NOTIFICATIONS) {
    // Children: [0]=placeholder(hidden), [1]=oldest bubble …
    uint32_t child_cnt = lv_obj_get_child_count(notif_scroll_cont);
    if (child_cnt > 1) {
      lv_obj_del(lv_obj_get_child(notif_scroll_cont, 1));
    }
  } else {
    notif_count++;
  }

  // ── Create bubble ──────────────────────────────────────────────
  lv_obj_t* bubble = lv_obj_create(notif_scroll_cont);
  lv_obj_set_width(bubble, 375);
  lv_obj_set_height(bubble, LV_SIZE_CONTENT);  // auto height — no clipping
  lv_obj_set_style_radius(bubble, 16, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bubble, lv_color_hex(0x1A1B2E), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_color(bubble, lv_color_hex(0x2A2B50), LV_PART_MAIN);
  lv_obj_set_style_border_width(bubble, 1, LV_PART_MAIN);
  lv_obj_set_style_pad_all(bubble, 14, LV_PART_MAIN);
  lv_obj_set_style_pad_row(bubble, 8, LV_PART_MAIN);
  lv_obj_clear_flag(bubble, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_flex_flow(bubble, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(bubble, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START);

  // App / sender label (blue) — extended font covers ä ö ü in names
  lv_obj_t* app_lbl = lv_label_create(bubble);
  lv_label_set_text(app_lbl, app);
  lv_label_set_long_mode(app_lbl, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(app_lbl, LV_PCT(100));
  lv_obj_set_style_text_font(app_lbl, &lv_font_montserrat_14_ext, LV_PART_MAIN);
  lv_obj_set_style_text_color(app_lbl, lv_color_hex(0x00aaff), LV_PART_MAIN);

  // Message body (white, full wrap, extended font for umlauts)
  lv_obj_t* msg_lbl = lv_label_create(bubble);
  lv_label_set_text(msg_lbl, msg);
  lv_label_set_long_mode(msg_lbl, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(msg_lbl, LV_PCT(100));
  lv_obj_set_style_text_font(msg_lbl, &lv_font_montserrat_16_ext, LV_PART_MAIN);
  lv_obj_set_style_text_color(msg_lbl, lv_color_white(), LV_PART_MAIN);

  // Scroll to bottom so newest notification is always visible
  lv_obj_scroll_to_y(notif_scroll_cont, LV_COORD_MAX, LV_ANIM_OFF);
}

void screens_set_ble_status(bool connected) {
  if (!ble_status_label) return;
  if (connected) {
    lv_label_set_text(ble_status_label, "BLE: Verbunden");
    lv_obj_set_style_text_color(ble_status_label, lv_color_hex(0x00ff88),
                                LV_PART_MAIN);
  } else {
    lv_label_set_text(ble_status_label, "BLE: Getrennt");
    lv_obj_set_style_text_color(ble_status_label, lv_color_hex(0x555555),
                                LV_PART_MAIN);
  }
}
