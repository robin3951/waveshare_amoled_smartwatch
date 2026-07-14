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

// styles specific to clock tile
lv_style_t _style_time_hours_minutes_label = {0};
lv_style_t _style_time_seconds_label = {0};
lv_style_t _style_date_label = {0};
lv_style_t _style_shoe_print_icon = {0};
lv_style_t _style_step_count_label = {0};

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

void init_clock_styles(void) {
  lv_style_init(&_style_time_hours_minutes_label);
  lv_style_set_text_font(&_style_time_hours_minutes_label,
                         &lv_font_montserrat_164);
  lv_style_set_text_color(&_style_time_hours_minutes_label, lv_color_white());

  lv_style_init(&_style_time_seconds_label);
  lv_obj_set_style_text_font(time_seconds_label, &lv_font_montserrat_48,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(time_seconds_label, lv_color_white(),
                              LV_PART_MAIN);

  lv_style_init(&_style_date_label);
  lv_obj_set_style_text_font(date_label, &lv_font_montserrat_32, LV_PART_MAIN);
  lv_obj_set_style_text_color(date_label, lv_color_hex(0x888888), LV_PART_MAIN);

  lv_style_init(&_style_shoe_print_icon);
  lv_obj_set_style_text_font(icon_shoe_print, &lv_font_shoe_print_48,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(icon_shoe_print, lv_color_hex(0x008000),
                              LV_PART_MAIN);

  lv_style_init(&_style_step_count_label);
  lv_obj_set_style_text_font(label_step_count, &lv_font_montserrat_32,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(label_step_count, lv_color_hex(0x888888),
                              LV_PART_MAIN);
}

void init_notification_styles(void) {
  lv_style_init(&_style_ble_status_label);
  lv_obj_set_style_text_font(ble_status_label, &lv_font_montserrat_12,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(ble_status_label, lv_color_hex(0x555555),
                              LV_PART_MAIN);

  lv_style_init(&_style_notification_scroll_content);
  lv_obj_set_style_bg_color(notif_scroll_cont, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(notif_scroll_cont, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(notif_scroll_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(notif_scroll_cont, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_row(notif_scroll_cont, 10, LV_PART_MAIN);

  lv_style_init(&_style_notification_empty_label);
  lv_obj_set_style_text_font(notif_empty_label, &lv_font_montserrat_16,
                             LV_PART_MAIN);
  lv_obj_set_style_text_color(notif_empty_label, lv_color_hex(0x444444),
                              LV_PART_MAIN);
}

void styles_init(void) {
  init_shared_styles();
  init_clock_styles();
  init_notification_styles();
}

lv_style_t* styles_tile_bg(void) { return &_style_tile_bg; }

lv_style_t* styles_time_hours_minutes_label(void) {
  return &_style_time_hours_minutes_label;
}

lv_style_t* styles_time_seconds_label(void) {
  return &_style_time_seconds_label;
}

lv_style_t* styles_date_label(void) { return &_style_date_label; }

lv_style_t* styles_shoe_print_icon(void) { return &_style_shoe_print_icon; }

lv_style_t* styles_step_count_label(void) { return &_style_step_count_label; }

lv_style_t* styles_ble_status_label(void) { return &_style_ble_status_label; }

lv_style_t* styles_notification_scroll_content(void) {
  return &_style_notification_scroll_content;
}

lv_style_t* styles_notification_empty_label(void) {
  return &_style_notification_empty_label;
}