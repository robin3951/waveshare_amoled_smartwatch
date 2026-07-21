/**
 * @file clock_screen.c
 * @author @robin3951
 * @brief Implementation of the clock screen for the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-07-14
 *
 */
#include "clock_screen.h"

#include "styles.h"

static lv_obj_t* clock_tile = NULL;

static lv_obj_t* time_hours_label = NULL;
static lv_obj_t* time_minutes_label = NULL;
static lv_obj_t* time_seconds_label = NULL;
static lv_obj_t* date_label = NULL;
static lv_obj_t* icon_shoe_print = NULL;
static lv_obj_t* label_step_count = NULL;

void create_clock_tile(lv_obj_t* clock_tile) {
  lv_obj_add_style(clock_tile, styles_tile_bg(), LV_PART_MAIN);

  time_hours_label = lv_label_create(clock_tile);
  lv_label_set_text(time_hours_label, "--");
  lv_obj_add_style(time_hours_label, styles_time_hours_minutes_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_hours_label, LV_ALIGN_CENTER, 0, -110);

  time_minutes_label = lv_label_create(clock_tile);
  lv_label_set_text(time_minutes_label, "--");
  lv_obj_add_style(time_minutes_label, styles_time_hours_minutes_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_minutes_label, LV_ALIGN_CENTER, 0, 30);

  time_seconds_label = lv_label_create(clock_tile);
  lv_label_set_text(time_seconds_label, "--");
  lv_obj_add_style(time_seconds_label, styles_time_seconds_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_seconds_label, LV_ALIGN_RIGHT_MID, -40, 60);

  date_label = lv_label_create(clock_tile);
  lv_label_set_text(date_label, "--.--.----");
  lv_obj_add_style(date_label, styles_date_label(), LV_PART_MAIN);
  lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, -110);

  icon_shoe_print = lv_label_create(clock_tile);
  lv_label_set_text(icon_shoe_print,
                    "\xEF\x95\x8B");  // Unicode for shoe print icon
  lv_obj_add_style(icon_shoe_print, styles_shoe_print_icon(), LV_PART_MAIN);
  lv_obj_align(icon_shoe_print, LV_ALIGN_BOTTOM_MID, -30, -50);

  label_step_count = lv_label_create(clock_tile);
  lv_label_set_text(label_step_count, "---");
  lv_obj_add_style(label_step_count, styles_step_count_label(), LV_PART_MAIN);
  lv_obj_align(label_step_count, LV_ALIGN_BOTTOM_MID, 30, -50);
}

const char* get_month_name(int month) {
  static const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  return month_names[month - 1];
}

void clock_screen_set_time(int hours, int minutes, int seconds) {
  if (time_hours_label) lv_label_set_text_fmt(time_hours_label, "%02d", hours);
  if (time_minutes_label)
    lv_label_set_text_fmt(time_minutes_label, "%02d", minutes);
  if (time_seconds_label)
    lv_label_set_text_fmt(time_seconds_label, "%02d", seconds);
}