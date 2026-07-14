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

lv_obj_t* ui_tile_notification = NULL;
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