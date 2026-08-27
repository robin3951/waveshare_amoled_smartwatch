// UI screens — tileview-based 3-screen horizontal swipe navigation
// Display: 410 x 502 px  (width x height)
#include "screens.h"

#include <string.h>

#include "battery_screen.h"
#include "clock_screen.h"
#include "lvgl.h"  // IWYU pragma: keep
#include "notification_screen.h"
#include "statusbar.h"
#include "styles.h"

static lv_obj_t* ui_tileview = NULL;

static lv_obj_t* ui_tile_clock = NULL;
static lv_obj_t* ui_tile_battery = NULL;
static lv_obj_t* ui_tile_notification = NULL;

static lv_obj_t* dot_container = NULL;
static lv_obj_t* dots[UI_SCREEN_COUNT];

static void update_dots(uint8_t idx) {
  for (uint8_t i = 0; i < UI_SCREEN_COUNT; i++) {
    if (i == idx) {
      lv_obj_set_size(dots[i], DOT_ACTIVE_SIZE, DOT_ACTIVE_SIZE);
      lv_obj_add_style(dots[i], styles_dot_active(), LV_PART_MAIN);
    } else {
      lv_obj_set_size(dots[i], DOT_INACTIVE_SIZE, DOT_INACTIVE_SIZE);
      lv_obj_add_style(dots[i], styles_dot_inactive(), LV_PART_MAIN);
    }
  }
}

static void tileview_changed_cb(lv_event_t* e) {
  lv_obj_t* tv = lv_event_get_target(e);
  lv_obj_t* act = lv_tileview_get_tile_active(tv);
  uint8_t idx = UI_TILE_CLOCK_IDX;
  if (act == ui_tile_battery)
    idx = UI_TILE_BATTERY_IDX;
  else if (act == ui_tile_notification)
    idx = UI_TILE_NOTIFICATION_IDX;
  update_dots(idx);
}

/* ─── Dot indicator overlay ──────────────────────────────────────── */

static void create_dot_indicator(lv_obj_t* screen) {
  dot_container = lv_obj_create(screen);
  lv_obj_add_style(dot_container, styles_dot_container(), LV_PART_MAIN);
  lv_obj_set_layout(dot_container, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(dot_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(dot_container, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(dot_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(dot_container, LV_ALIGN_BOTTOM_MID, 0, DOT_INDICATOR_Y_OFFSET);
  lv_obj_clear_flag(dot_container,
                    LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

  for (uint8_t i = 0; i < UI_SCREEN_COUNT; i++) {
    dots[i] = lv_obj_create(dot_container);
    lv_obj_add_style(dots[i], styles_dot_inactive(), LV_PART_MAIN);
    lv_obj_set_size(dots[i], DOT_INACTIVE_SIZE, DOT_INACTIVE_SIZE);
    lv_obj_clear_flag(dots[i], LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
  }
  update_dots(0);
}

/* ─── Public: create all screens ────────────────────────────────── */

void create_screens(void) {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_add_style(screen, styles_tile_bg(), LV_PART_MAIN);

  ui_tileview = lv_tileview_create(screen);
  lv_obj_set_scrollbar_mode(ui_tileview, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_style(ui_tileview, styles_tile_bg(), LV_PART_MAIN);

  ui_tile_clock =
      lv_tileview_add_tile(ui_tileview, UI_TILE_CLOCK_IDX, 0, LV_DIR_HOR);
  ui_tile_battery =
      lv_tileview_add_tile(ui_tileview, UI_TILE_BATTERY_IDX, 0, LV_DIR_HOR);
  ui_tile_notification = lv_tileview_add_tile(
      ui_tileview, UI_TILE_NOTIFICATION_IDX, 0, LV_DIR_HOR);

  create_clock_tile(ui_tile_clock);
  create_battery_tile(ui_tile_battery);
  create_notification_tile(ui_tile_notification);

  create_dot_indicator(screen);
  create_statusbar();

  lv_obj_add_event_cb(ui_tileview, tileview_changed_cb, LV_EVENT_VALUE_CHANGED,
                      NULL);
}

/* ─── Public: data update functions ─────────────────────────────── */

void screens_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  clock_screen_set_time(hours, minutes, seconds);
}

void screens_set_date(uint8_t day, uint8_t month, uint16_t year) {
  clock_screen_set_date(day, month, year);
}

void screens_set_steps(uint32_t steps) { clock_screen_set_steps(steps); }

void screens_set_battery(uint8_t percent, bool charging, float voltage) {
  battery_screen_set_battery(percent, charging, voltage);
}

void screens_add_notification(const char* app, const char* message) {
  notification_screen_add_notification(app, message);
}

void screens_set_ble_status(bool connected) {
  notification_screen_set_ble_status(connected);
}
