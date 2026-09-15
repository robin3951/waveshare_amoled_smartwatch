/**
 * @file statusbar.c
 * @author @robin3951
 * @brief Implementation of the statusbar overlay for the Waveshare ESP32-S3
 * AMOLED Smartwatch firmware.
 * @date 2026-08-18
 *
 */

#include "statusbar.h"

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

static lv_obj_t* statusbar_container = NULL;

static lv_obj_t* battery_status_container = NULL;
static lv_obj_t* battery_icon = NULL;
static lv_obj_t* battery_charging_icon = NULL;
static lv_obj_t* battery_percentage_label = NULL;

static lv_obj_t* bluetooth_status_container = NULL;
static lv_obj_t* bluetooth_icon = NULL;

static lv_obj_t* wifi_status_container = NULL;
static lv_obj_t* wifi_icon = NULL;

static lv_obj_t* speaker_status_container = NULL;
static lv_obj_t* speaker_icon = NULL;

static lv_obj_t* notification_status_container = NULL;
static lv_obj_t* notification_icon = NULL;
static lv_obj_t* notification_count_container = NULL;
static lv_obj_t* notification_count_label = NULL;

static lv_obj_t* create_battery_status_container(lv_obj_t* parent) {
  battery_status_container = lv_obj_create(parent);
  lv_obj_set_width(battery_status_container, LV_SIZE_CONTENT);
  lv_obj_set_height(battery_status_container, LV_SIZE_CONTENT);
  lv_obj_add_style(battery_status_container, styles_battery_status_container(),
                   LV_PART_MAIN);
  lv_obj_set_flex_flow(battery_status_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(battery_status_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  battery_icon = lv_label_create(battery_status_container);
  lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
  lv_obj_add_style(battery_icon, styles_battery_icon(), LV_PART_MAIN);

  battery_charging_icon = lv_label_create(battery_status_container);
  lv_label_set_text(battery_charging_icon, LV_SYMBOL_CHARGE);
  lv_obj_add_style(battery_charging_icon, styles_battery_charging_icon(),
                   LV_PART_MAIN);

  battery_percentage_label = lv_label_create(battery_status_container);
  lv_label_set_text(battery_percentage_label, "100%");
  lv_obj_add_style(battery_percentage_label, styles_battery_percentage_label(),
                   LV_PART_MAIN);
  return battery_status_container;
}

static lv_obj_t* create_bluetooth_status_container(lv_obj_t* parent) {
  bluetooth_status_container = lv_obj_create(parent);
  lv_obj_set_width(bluetooth_status_container, LV_SIZE_CONTENT);
  lv_obj_set_height(bluetooth_status_container, LV_SIZE_CONTENT);
  lv_obj_add_style(bluetooth_status_container,
                   styles_bluetooth_status_container(), LV_PART_MAIN);
  lv_obj_set_flex_flow(bluetooth_status_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bluetooth_status_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  bluetooth_icon = lv_label_create(bluetooth_status_container);
  lv_label_set_text(bluetooth_icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_add_style(bluetooth_icon, styles_bluetooth_icon(), LV_PART_MAIN);
  return bluetooth_status_container;
}

static lv_obj_t* create_wifi_status_container(lv_obj_t* parent) {
  wifi_status_container = lv_obj_create(parent);
  lv_obj_set_width(wifi_status_container, LV_SIZE_CONTENT);
  lv_obj_set_height(wifi_status_container, LV_SIZE_CONTENT);
  lv_obj_add_style(wifi_status_container, styles_wifi_status_container(),
                   LV_PART_MAIN);
  lv_obj_set_flex_flow(wifi_status_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(wifi_status_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  wifi_icon = lv_label_create(wifi_status_container);
  lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
  lv_obj_add_style(wifi_icon, styles_wifi_icon(), LV_PART_MAIN);
  return wifi_status_container;
}

static lv_obj_t* create_speaker_status_container(lv_obj_t* parent) {
  speaker_status_container = lv_obj_create(parent);
  lv_obj_set_width(speaker_status_container, LV_SIZE_CONTENT);
  lv_obj_set_height(speaker_status_container, LV_SIZE_CONTENT);
  lv_obj_add_style(speaker_status_container, styles_speaker_status_container(),
                   LV_PART_MAIN);
  lv_obj_set_flex_flow(speaker_status_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(speaker_status_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  speaker_icon = lv_label_create(speaker_status_container);
  lv_label_set_text(speaker_icon, LV_SYMBOL_VOLUME_MAX);
  lv_obj_add_style(speaker_icon, styles_speaker_icon(), LV_PART_MAIN);
  return speaker_status_container;
}

static lv_obj_t* create_notification_status_container(lv_obj_t* parent) {
  notification_status_container = lv_obj_create(parent);
  lv_obj_set_width(notification_status_container, LV_SIZE_CONTENT);
  lv_obj_set_height(notification_status_container, LV_SIZE_CONTENT);
  lv_obj_add_style(notification_status_container,
                   styles_notification_status_container(), LV_PART_MAIN);
  lv_obj_set_flex_flow(notification_status_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(notification_status_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  notification_icon = lv_label_create(notification_status_container);
  lv_label_set_text(notification_icon, LV_SYMBOL_BELL);
  lv_obj_add_style(notification_icon, styles_notification_icon(), LV_PART_MAIN);

  notification_count_container = lv_obj_create(notification_status_container);
  lv_obj_set_width(notification_count_container, LV_SIZE_CONTENT);
  lv_obj_set_height(notification_count_container, LV_SIZE_CONTENT);
  lv_obj_add_style(notification_count_container,
                   styles_notification_count_container(), LV_PART_MAIN);

  notification_count_label = lv_label_create(notification_count_container);
  lv_obj_add_style(notification_count_label, styles_notification_count_label(),
                   LV_PART_MAIN);
  lv_label_set_text(notification_count_label, "14");
  return notification_status_container;
}

void create_statusbar(void) {
  statusbar_container = lv_obj_create(lv_layer_top());
  lv_obj_set_width(statusbar_container, STATUSBAR_CONTAINER_WIDTH);
  lv_obj_set_height(statusbar_container, LV_SIZE_CONTENT);
  lv_obj_add_style(statusbar_container, styles_statusbar_container(),
                   LV_PART_MAIN);
  lv_obj_set_flex_flow(statusbar_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(statusbar_container, LV_FLEX_ALIGN_SPACE_AROUND,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  battery_status_container =
      create_battery_status_container(statusbar_container);
  bluetooth_status_container =
      create_bluetooth_status_container(statusbar_container);
  wifi_status_container = create_wifi_status_container(statusbar_container);
  speaker_status_container =
      create_speaker_status_container(statusbar_container);
  notification_status_container =
      create_notification_status_container(statusbar_container);
}

static const char* get_battery_icon(const battery_status_t* battery_status) {
  uint8_t battery_percentage = battery_status->percent;
  if (battery_percentage >= BATTERY_FULL_THRESHOLD)
    return LV_SYMBOL_BATTERY_FULL;
  if (battery_percentage >= BATTERY_HIGH_THRESHOLD) return LV_SYMBOL_BATTERY_3;
  if (battery_percentage >= BATTERY_MEDIUM_THRESHOLD)
    return LV_SYMBOL_BATTERY_2;
  if (battery_percentage >= BATTERY_LOW_THRESHOLD) return LV_SYMBOL_BATTERY_1;
  return LV_SYMBOL_BATTERY_EMPTY;
}

static lv_color_t get_battery_color(const battery_status_t* battery_status) {
  uint8_t battery_percentage = battery_status->percent;
  bool battery_is_charging = battery_status->charging;
  if (battery_is_charging) return lv_color_hex(BATTERY_CHARGING_COLOR_HEX);
  if (battery_percentage < BATTERY_LOW_THRESHOLD)
    return lv_color_hex(BATTERY_LOW_COLOR_HEX);
  if (battery_percentage < BATTERY_MEDIUM_THRESHOLD)
    return lv_color_hex(BATTERY_MEDIUM_COLOR_HEX);
  return lv_color_white();
}

void statusbar_set_battery_status(const battery_status_t* battery_status) {
  if (!battery_status_container) return;

  if (battery_status->charging) {
    lv_obj_clear_flag(battery_charging_icon, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(battery_charging_icon, LV_OBJ_FLAG_HIDDEN);
  }

  uint8_t battery_percentage = battery_status->percent;
  lv_label_set_text_fmt(battery_percentage_label, "%d%%", battery_percentage);
  lv_obj_set_style_text_color(battery_percentage_label,
                              get_battery_color(battery_status), LV_PART_MAIN);

  lv_label_set_text(battery_icon, get_battery_icon(battery_status));
  lv_obj_set_style_text_color(battery_icon, get_battery_color(battery_status),
                              LV_PART_MAIN);
}

void statusbar_set_bluetooth_status() {}

void statusbar_set_wifi_status() {}

void statusbar_set_speaker_status() {}

void statusbar_set_notification_status() {}