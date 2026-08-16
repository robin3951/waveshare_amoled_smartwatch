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

static lv_obj_t* ble_status_label = NULL;
static lv_obj_t* notification_empty_label = NULL;
static lv_obj_t* notification_scroll_content = NULL;
static lv_obj_t* notification_bubble = NULL;
static lv_obj_t* notification_app_label = NULL;
static lv_obj_t* notification_message_label = NULL;

uint8_t notification_count = 0;

void create_notification_tile(lv_obj_t* notification_tile) {
  lv_obj_add_style(notification_tile, styles_tile_bg(), LV_PART_MAIN);

  ble_status_label = lv_label_create(notification_tile);
  lv_obj_set_pos(ble_status_label, 0, BLE_STATUS_LABEL_POS_Y);
  lv_obj_set_width(ble_status_label, BLE_STATUS_LABEL_WIDTH);
  lv_obj_add_style(ble_status_label, styles_ble_status_label(), LV_PART_MAIN);
  lv_label_set_text(ble_status_label, BLE_STATUS_LABEL_DISCONNECTED_TEXT);

  notification_scroll_content = lv_obj_create(notification_tile);
  lv_obj_set_pos(notification_scroll_content, 0,
                 NOTIFICATION_SCROLL_CONTENT_POS_Y);
  lv_obj_set_size(notification_scroll_content,
                  NOTIFICATION_SCROLL_CONTENT_WIDTH,
                  NOTIFICATION_SCROLL_CONTENT_HEIGHT);
  lv_obj_add_style(notification_scroll_content,
                   styles_notification_scroll_content(), LV_PART_MAIN);
  lv_obj_set_scroll_dir(notification_scroll_content, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(notification_scroll_content,
                            LV_SCROLLBAR_MODE_ACTIVE);

  lv_obj_set_flex_flow(notification_scroll_content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(notification_scroll_content, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

  notification_empty_label = lv_label_create(notification_scroll_content);
  lv_obj_add_style(notification_empty_label, styles_notification_empty_label(),
                   LV_PART_MAIN);
  lv_label_set_text(notification_empty_label, NOTIFICATION_EMPTY_LABEL_TEXT);
  lv_obj_set_style_text_align(notification_empty_label, LV_TEXT_ALIGN_CENTER,
                              LV_PART_MAIN);
  lv_obj_set_width(notification_empty_label, NOTIFICATION_EMPTY_LABEL_WIDTH);
  lv_obj_align(notification_empty_label, LV_ALIGN_CENTER, 0, 0);
}

void notification_screen_add_notification(const char* app,
                                          const char* message) {
  if (!notification_scroll_content) return;
  if (notification_count == 0 && notification_empty_label) {
    lv_obj_add_flag(notification_empty_label, LV_OBJ_FLAG_HIDDEN);
  }
  if (notification_count >= MAX_NOTIFICATIONS) {
    uint32_t child_count = lv_obj_get_child_count(notification_scroll_content);
    if (child_count > 1) {
      lv_obj_del(lv_obj_get_child(notification_scroll_content, 1));
    }
  } else {
    notification_count++;
  }

  notification_bubble = lv_obj_create(notification_scroll_content);
  lv_obj_add_style(notification_bubble, styles_notification_bubble(),
                   LV_PART_MAIN);
  lv_obj_set_width(notification_bubble, NOTIFICATION_BUBBLE_WIDTH);
  lv_obj_set_height(notification_bubble, LV_SIZE_CONTENT);

  lv_obj_clear_flag(notification_bubble,
                    LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_flex_flow(notification_bubble, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(notification_bubble, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

  notification_app_label = lv_label_create(notification_bubble);
  lv_obj_add_style(notification_app_label, styles_notification_app_label(),
                   LV_PART_MAIN);
  lv_label_set_text(notification_app_label, app);
  lv_label_set_long_mode(notification_app_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(notification_app_label, LV_PCT(100));

  notification_message_label = lv_label_create(notification_bubble);
  lv_obj_add_style(notification_message_label,
                   styles_notification_message_label(), LV_PART_MAIN);
  lv_label_set_text(notification_message_label, message);
  lv_label_set_long_mode(notification_message_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(notification_message_label, LV_PCT(100));
  lv_obj_scroll_to_y(notification_scroll_content, LV_COORD_MAX, LV_ANIM_OFF);
}

void notification_screen_set_ble_status(bool connected) {
  if (!ble_status_label) return;
  if (connected) {
    lv_label_set_text(ble_status_label, BLE_STATUS_LABEL_CONNECTED_TEXT);
    lv_obj_add_style(ble_status_label, styles_ble_status_connected(),
                     LV_PART_MAIN);
  } else {
    lv_label_set_text(ble_status_label, BLE_STATUS_LABEL_DISCONNECTED_TEXT);
    lv_obj_add_style(ble_status_label, styles_ble_status_disconnected(),
                     LV_PART_MAIN);
  }
}