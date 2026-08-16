/**
 * @file notification_screen.c
 * @author @robin3951
 * @brief Implementation of the notification screen for the Waveshare ESP32-S3
 * AMOLED Smartwatch firmware.
 * @date 2026-07-14
 *
 */

#include "notification_screen.h"

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

static lv_obj_t* notification_tile = NULL;

lv_obj_t* ble_status_label = NULL;
lv_obj_t* notification_empty_label = NULL;
lv_obj_t* notification_scroll_content = NULL;  // scrollable bubble container

int notification_count = 0;  // number of active notification bubbles

void create_notification_tile(lv_obj_t* notification_tile) {
  lv_obj_add_style(notification_tile, styles_tile_bg(), LV_PART_MAIN);

  // BLE status line at top (fixed, 30 px)
  ble_status_label = lv_label_create(notification_tile);
  lv_obj_set_pos(ble_status_label, 0, 6);
  lv_obj_set_width(ble_status_label, 410);
  lv_obj_add_style(ble_status_label, styles_ble_status_label(), LV_PART_MAIN);
  lv_label_set_text(ble_status_label, "BLE: Getrennt");

  // Scrollable bubble container fills the remaining height (502 - 30 = 472 px)
  notification_scroll_content = lv_obj_create(notification_tile);
  lv_obj_set_pos(notification_scroll_content, 0, 30);
  lv_obj_set_size(notification_scroll_content, 410, 456);
  lv_obj_add_style(notification_scroll_content,
                   styles_notification_scroll_content(), LV_PART_MAIN);
  lv_obj_set_scroll_dir(notification_scroll_content, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(notification_scroll_content,
                            LV_SCROLLBAR_MODE_ACTIVE);
  // Flex column: children stacked vertically, centered horizontally
  lv_obj_set_flex_flow(notification_scroll_content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(notification_scroll_content,
                        LV_FLEX_ALIGN_START,   // main axis: top-to-bottom
                        LV_FLEX_ALIGN_CENTER,  // cross axis: centered
                        LV_FLEX_ALIGN_START);

  // "No notifications" placeholder — centered, hidden once first bubble arrives
  notification_empty_label = lv_label_create(notification_scroll_content);
  lv_obj_add_style(notification_empty_label, styles_notification_empty_label(),
                   LV_PART_MAIN);
  lv_label_set_text(notification_empty_label, "Keine\nBenachrichtigungen");
  lv_obj_set_style_text_align(notification_empty_label, LV_TEXT_ALIGN_CENTER,
                              LV_PART_MAIN);
  lv_obj_set_width(notification_empty_label, 410);
  lv_obj_align(notification_empty_label, LV_ALIGN_CENTER, 0, 0);
}

void notification_screen_add_notification(const char* app,
                                          const char* message) {
  if (!notification_scroll_content) return;

  // Hide "empty" placeholder once first notification arrives
  if (notification_count == 0 && notification_empty_label) {
    lv_obj_add_flag(notification_empty_label, LV_OBJ_FLAG_HIDDEN);
  }

  // If limit reached, delete the oldest bubble (first real child after
  // placeholder)
  if (notification_count >= MAX_NOTIFICATIONS) {
    // Children: [0]=placeholder(hidden), [1]=oldest bubble …
    uint32_t child_cnt = lv_obj_get_child_count(notification_scroll_content);
    if (child_cnt > 1) {
      lv_obj_del(lv_obj_get_child(notification_scroll_content, 1));
    }
  } else {
    notification_count++;
  }

  // ── Create bubble ──────────────────────────────────────────────
  lv_obj_t* bubble = lv_obj_create(notification_scroll_content);
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
  lv_label_set_text(msg_lbl, message);
  lv_label_set_long_mode(msg_lbl, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(msg_lbl, LV_PCT(100));
  lv_obj_set_style_text_font(msg_lbl, &lv_font_montserrat_16_ext, LV_PART_MAIN);
  lv_obj_set_style_text_color(msg_lbl, lv_color_white(), LV_PART_MAIN);

  // Scroll to bottom so newest notification is always visible
  lv_obj_scroll_to_y(notification_scroll_content, LV_COORD_MAX, LV_ANIM_OFF);
}

void notification_screen_set_ble_status(bool connected) {
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