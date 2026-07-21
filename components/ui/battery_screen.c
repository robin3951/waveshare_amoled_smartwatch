#include "battery_screen.h"

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

static lv_obj_t* battery_tile = NULL;

static lv_obj_t* battery_icon_label = NULL;
static lv_obj_t* battery_percent_label = NULL;
static lv_obj_t* battery_status_label = NULL;
static lv_obj_t* battery_voltage_label = NULL;

static void create_battery_tile(lv_obj_t* battery_tile) {
  lv_obj_add_style(battery_tile, styles_tile_bg(), LV_PART_MAIN);

  battery_icon_label = lv_label_create(battery_tile);
  lv_label_set_text(battery_icon_label, "---");
  lv_obj_add_style(battery_icon_label, styles_battery_icon_label(),
                   LV_PART_MAIN);
  lv_obj_align(battery_icon_label, LV_ALIGN_TOP_RIGHT, -10, 10);

  battery_percent_label = lv_label_create(battery_tile);
  lv_label_set_text(battery_percent_label, "--%");
  lv_obj_add_style(battery_percent_label, styles_battery_percent_label(),
                   LV_PART_MAIN);
  lv_obj_align(battery_percent_label, LV_ALIGN_CENTER, 0, -20);

  battery_status_label = lv_label_create(battery_tile);
  lv_label_set_text(battery_status_label, "");
  lv_obj_add_style(battery_status_label, styles_battery_status_label(),
                   LV_PART_MAIN);
  lv_obj_align(battery_status_label, LV_ALIGN_CENTER, 0, 45);

  battery_voltage_label = lv_label_create(battery_tile);
  lv_label_set_text(battery_voltage_label, "-.-- V");
  lv_obj_add_style(battery_voltage_label, styles_battery_voltage_label(),
                   LV_PART_MAIN);
  lv_obj_align(battery_voltage_label, LV_ALIGN_CENTER, 0, 85);
}

static const char* get_battery_icon(int percent, bool charging) {
  if (charging) return "CHG";
  if (percent >= 90) return "100";
  if (percent >= 70) return " 75";
  if (percent >= 45) return " 50";
  if (percent >= 20) return " 25";
  return "LOW";
}

void battery_screen_set_battery(int percent, bool charging, float voltage) {
  if (!battery_percent_label) return;

  lv_color_t color;
  if (charging)
    color = lv_color_hex(0x00ff88);
  else if (percent < 20)
    color = lv_color_hex(0xff3333);
  else if (percent < 50)
    color = lv_color_hex(0xffaa00);
  else
    color = lv_color_white();

  lv_label_set_text(battery_icon_label, get_battery_icon(percent, charging));
  lv_obj_set_style_text_color(battery_icon_label, color, LV_PART_MAIN);

  lv_label_set_text_fmt(battery_percent_label, "%d%%", percent);
  lv_obj_set_style_text_color(battery_percent_label, color, LV_PART_MAIN);

  if (charging) {
    lv_label_set_text(battery_status_label, "Laden");
    lv_obj_set_style_text_color(battery_status_label, lv_color_hex(0x00ff88),
                                LV_PART_MAIN);
  } else {
    lv_label_set_text(battery_status_label, "");
  }

  lv_label_set_text_fmt(battery_voltage_label, "%.2f V", voltage);
  lv_obj_set_style_text_color(battery_voltage_label, lv_color_hex(0x555555),
                              LV_PART_MAIN);
}