/**
 * @file styles.c
 * @author @robin3951
 * @brief Implementation of LVGL UI styles used in the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-07-14
 *
 */
#include "styles.h"

#include "fonts/fonts.h"

// styles shared across all tiles
lv_style_t _style_tile_bg = {0};

lv_style_t _style_dot_container = {0};
lv_style_t _style_dot_inactive = {0};
lv_style_t _style_dot_active = {0};

// styles specific to clock tile
lv_style_t _style_time_hours_minutes_label = {0};
lv_style_t _style_time_seconds_label = {0};
lv_style_t _style_date_label = {0};
lv_style_t _style_shoe_print_icon = {0};
lv_style_t _style_step_count_label = {0};

// styles specific to battery tile
lv_style_t _style_battery_icon_label = {0};
lv_style_t _style_battery_percent_label = {0};
lv_style_t _style_battery_status_label = {0};
lv_style_t _style_battery_voltage_label = {0};

// styles specific to notification tile
lv_style_t _style_ble_status_label = {0};
lv_style_t _style_notification_scroll_content = {0};
lv_style_t _style_notification_empty_label = {0};

void init_shared_styles(void) {
  lv_style_init(&_style_tile_bg);
  lv_style_set_bg_color(&_style_tile_bg, lv_color_black());
  lv_style_set_bg_opa(&_style_tile_bg, LV_OPA_COVER);
  lv_style_set_border_width(&_style_tile_bg, 0);
  lv_style_set_pad_all(&_style_tile_bg, 0);
}

void init_dot_indicator_styles(void) {
  lv_style_init(&_style_dot_container);
  lv_style_set_bg_opa(&_style_dot_container, LV_OPA_TRANSP);
  lv_style_set_border_width(&_style_dot_container, 0);
  lv_style_set_pad_all(&_style_dot_container, 4);
  lv_style_set_pad_column(&_style_dot_container, 8);

  lv_style_init(&_style_dot_inactive);
  lv_style_set_radius(&_style_dot_inactive, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&_style_dot_inactive, lv_color_hex(0x555555));
  lv_style_set_bg_opa(&_style_dot_inactive, LV_OPA_COVER);
  lv_style_set_border_width(&_style_dot_inactive, 0);
  lv_style_set_pad_all(&_style_dot_inactive, 0);

  lv_style_init(&_style_dot_active);
  lv_style_set_radius(&_style_dot_active, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&_style_dot_active, lv_color_white());
  lv_style_set_bg_opa(&_style_dot_active, LV_OPA_COVER);
  lv_style_set_border_width(&_style_dot_active, 0);
  lv_style_set_pad_all(&_style_dot_active, 0);
}

void init_clock_styles(void) {
  lv_style_init(&_style_time_hours_minutes_label);
  lv_style_set_text_font(&_style_time_hours_minutes_label,
                         &lv_font_montserrat_164);
  lv_style_set_text_color(&_style_time_hours_minutes_label, lv_color_white());

  lv_style_init(&_style_time_seconds_label);
  lv_style_set_text_font(&_style_time_seconds_label, &lv_font_montserrat_48);
  lv_style_set_text_color(&_style_time_seconds_label, lv_color_white());

  lv_style_init(&_style_date_label);
  lv_style_set_text_font(&_style_date_label, &lv_font_montserrat_32);
  lv_style_set_text_color(&_style_date_label, lv_color_hex(0x888888));

  lv_style_init(&_style_shoe_print_icon);
  lv_style_set_text_font(&_style_shoe_print_icon, &lv_font_shoe_print_48);
  lv_style_set_text_color(&_style_shoe_print_icon, lv_color_hex(0x008000));

  lv_style_init(&_style_step_count_label);
  lv_style_set_text_font(&_style_step_count_label, &lv_font_montserrat_32);
  lv_style_set_text_color(&_style_step_count_label, lv_color_hex(0x888888));
}

void init_battery_styles(void) {
  lv_style_init(&_style_battery_icon_label);
  lv_style_set_text_font(&_style_battery_icon_label, &lv_font_montserrat_12);
  lv_style_set_text_color(&_style_battery_icon_label, lv_color_white());

  lv_style_init(&_style_battery_percent_label);
  lv_style_set_text_font(&_style_battery_percent_label, &lv_font_montserrat_48);
  lv_style_set_text_color(&_style_battery_percent_label, lv_color_white());

  lv_style_init(&_style_battery_status_label);
  lv_style_set_text_font(&_style_battery_status_label, &lv_font_montserrat_16);
  lv_style_set_text_color(&_style_battery_status_label, lv_color_hex(0x00ff88));

  lv_style_init(&_style_battery_voltage_label);
  lv_style_set_text_font(&_style_battery_voltage_label, &lv_font_montserrat_20);
  lv_style_set_text_color(&_style_battery_voltage_label,
                          lv_color_hex(0x555555));
}

void init_notification_styles(void) {
  lv_style_init(&_style_ble_status_label);
  lv_style_set_text_font(&_style_ble_status_label, &lv_font_montserrat_12);
  lv_style_set_text_color(&_style_ble_status_label, lv_color_hex(0x555555));

  lv_style_init(&_style_notification_scroll_content);
  lv_style_set_bg_color(&_style_notification_scroll_content, lv_color_black());
  lv_style_set_bg_opa(&_style_notification_scroll_content, LV_OPA_COVER);
  lv_style_set_border_width(&_style_notification_scroll_content, 0);
  lv_style_set_pad_all(&_style_notification_scroll_content, 10);
  lv_style_set_pad_row(&_style_notification_scroll_content, 10);

  lv_style_init(&_style_notification_empty_label);
  lv_style_set_text_font(&_style_notification_empty_label,
                         &lv_font_montserrat_16);
  lv_style_set_text_color(&_style_notification_empty_label,
                          lv_color_hex(0x444444));
}

void styles_init(void) {
  init_shared_styles();
  init_dot_indicator_styles();
  init_clock_styles();
  init_battery_styles();
  init_notification_styles();
}

lv_style_t* styles_tile_bg(void) { return &_style_tile_bg; }

lv_style_t* styles_dot_container(void) { return &_style_dot_container; }

lv_style_t* styles_dot_inactive(void) { return &_style_dot_inactive; }

lv_style_t* styles_dot_active(void) { return &_style_dot_active; }

lv_style_t* styles_time_hours_minutes_label(void) {
  return &_style_time_hours_minutes_label;
}

lv_style_t* styles_time_seconds_label(void) {
  return &_style_time_seconds_label;
}

lv_style_t* styles_date_label(void) { return &_style_date_label; }

lv_style_t* styles_shoe_print_icon(void) { return &_style_shoe_print_icon; }

lv_style_t* styles_step_count_label(void) { return &_style_step_count_label; }

lv_style_t* styles_battery_icon_label(void) {
  return &_style_battery_icon_label;
}

lv_style_t* styles_battery_percent_label(void) {
  return &_style_battery_percent_label;
}

lv_style_t* styles_battery_status_label(void) {
  return &_style_battery_status_label;
}

lv_style_t* styles_battery_voltage_label(void) {
  return &_style_battery_voltage_label;
}

lv_style_t* styles_ble_status_label(void) { return &_style_ble_status_label; }

lv_style_t* styles_notification_scroll_content(void) {
  return &_style_notification_scroll_content;
}

lv_style_t* styles_notification_empty_label(void) {
  return &_style_notification_empty_label;
}