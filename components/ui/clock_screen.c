/**
 * @file clock_screen.c
 * @author @robin3951
 * @brief Implementation of the clock screen for the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-07-14
 *
 */
#include "clock_screen.h"

#include "clock_task.h"
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
  lv_label_set_text(time_hours_label, CLOCK_TIME_DEFAULT_PLACEHOLDER);
  lv_obj_add_style(time_hours_label, styles_time_hours_minutes_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_hours_label, LV_ALIGN_CENTER, 0,
               CLOCK_TIME_HOURS_LABEL_Y_OFFSET);

  time_minutes_label = lv_label_create(clock_tile);
  lv_label_set_text(time_minutes_label, CLOCK_TIME_DEFAULT_PLACEHOLDER);
  lv_obj_add_style(time_minutes_label, styles_time_hours_minutes_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_minutes_label, LV_ALIGN_CENTER, 0,
               CLOCK_TIME_MINUTES_LABEL_Y_OFFSET);

  time_seconds_label = lv_label_create(clock_tile);
  lv_label_set_text(time_seconds_label, CLOCK_TIME_DEFAULT_PLACEHOLDER);
  lv_obj_add_style(time_seconds_label, styles_time_seconds_label(),
                   LV_PART_MAIN);
  lv_obj_align(time_seconds_label, LV_ALIGN_RIGHT_MID,
               CLOCK_TIME_SECONDS_LABEL_X_OFFSET,
               CLOCK_TIME_SECONDS_LABEL_Y_OFFSET);

  date_label = lv_label_create(clock_tile);
  lv_label_set_text(date_label, CLOCK_TIME_DEFAULT_PLACEHOLDER);
  lv_obj_add_style(date_label, styles_date_label(), LV_PART_MAIN);
  lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, CLOCK_DATE_LABEL_Y_OFFSET);

  icon_shoe_print = lv_label_create(clock_tile);
  lv_label_set_text(icon_shoe_print, ICON_SHOE_PRINT_UNICODE);
  lv_obj_add_style(icon_shoe_print, styles_shoe_print_icon(), LV_PART_MAIN);
  lv_obj_align(icon_shoe_print, LV_ALIGN_BOTTOM_MID, ICON_SHOE_PRINT_X_OFFSET,
               ICON_SHOE_PRINT_Y_OFFSET);

  label_step_count = lv_label_create(clock_tile);
  lv_label_set_text(label_step_count, STEP_COUNT_DEFAULT_PLACEHOLDER);
  lv_obj_add_style(label_step_count, styles_step_count_label(), LV_PART_MAIN);
  lv_obj_align(label_step_count, LV_ALIGN_BOTTOM_MID, STEP_COUNT_LABEL_X_OFFSET,
               STEP_COUNT_LABEL_Y_OFFSET);
}

const char* get_month_name(uint8_t month) {
  static const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  if (month < 1 || month > 12) return "Invalid";
  return month_names[month - 1];
}

void clock_screen_set_time(const clock_time_t* time) {
  if (time_hours_label)
    lv_label_set_text_fmt(time_hours_label, "%02d", time->hours);
  if (time_minutes_label)
    lv_label_set_text_fmt(time_minutes_label, "%02d", time->minutes);
  if (time_seconds_label)
    lv_label_set_text_fmt(time_seconds_label, "%02d", time->seconds);
}

void clock_screen_set_date(const date_time_t* date) {
  if (date_label)
    lv_label_set_text_fmt(date_label, "%02d %s %04d", date->day,
                          get_month_name(date->month), date->year);
}

void clock_screen_set_steps(uint32_t steps) {
  if (label_step_count)
    lv_label_set_text_fmt(label_step_count, "%lu", (unsigned long)steps);
}