// UI screens — tileview-based 3-screen horizontal swipe navigation
// Display: 410 x 502 px  (width x height)
#include "screens.h"

#include <string.h>

#include "battery_screen.h"
#include "clock_screen.h"
#include "lvgl.h"  // IWYU pragma: keep
#include "notification_screen.h"
#include "styles.h"

lv_obj_t* ui_tileview = NULL;
lv_obj_t* ui_tile_clock = NULL;
lv_obj_t* ui_tile_battery = NULL;
lv_obj_t* ui_tile_notification = NULL;

static lv_obj_t* dot_container = NULL;
static lv_obj_t* dots[UI_SCREEN_COUNT];

static void update_dots(int idx) {
  for (int i = 0; i < UI_SCREEN_COUNT; i++) {
    if (i == idx) {
      lv_obj_set_size(dots[i], 10, 10);
      lv_obj_add_style(dots[i], styles_dot_active(), LV_PART_MAIN);
    } else {
      lv_obj_set_size(dots[i], 8, 8);
      lv_obj_add_style(dots[i], styles_dot_inactive(), LV_PART_MAIN);
    }
  }
}

static void tileview_changed_cb(lv_event_t* e) {
  lv_obj_t* tv = lv_event_get_target(e);
  lv_obj_t* act = lv_tileview_get_tile_active(tv);
  int idx = 0;
  if (act == ui_tile_battery)
    idx = 1;
  else if (act == ui_tile_notification)
    idx = 2;
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
  lv_obj_align(dot_container, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_clear_flag(dot_container,
                    LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

  for (int i = 0; i < UI_SCREEN_COUNT; i++) {
    dots[i] = lv_obj_create(dot_container);
    lv_obj_add_style(dots[i], styles_dot_inactive(), LV_PART_MAIN);
    lv_obj_set_size(dots[i], 8, 8);
    lv_obj_clear_flag(dots[i], LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
  }
  update_dots(0);
}

/* ─── Public: create all screens ────────────────────────────────── */

void create_screens(void) {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  ui_tileview = lv_tileview_create(screen);
  lv_obj_set_scrollbar_mode(ui_tileview, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_color(ui_tileview, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(ui_tileview, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_pad_all(ui_tileview, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(ui_tileview, 0, LV_PART_MAIN);

  ui_tile_clock = lv_tileview_add_tile(ui_tileview, 0, 0, LV_DIR_HOR);
  ui_tile_battery = lv_tileview_add_tile(ui_tileview, 1, 0, LV_DIR_HOR);
  ui_tile_notification = lv_tileview_add_tile(ui_tileview, 2, 0, LV_DIR_HOR);

  create_clock_tile(ui_tile_clock);
  create_battery_tile(ui_tile_battery);
  create_notification_tile(ui_tile_notification);

  create_dot_indicator(screen);

  lv_obj_add_event_cb(ui_tileview, tileview_changed_cb, LV_EVENT_VALUE_CHANGED,
                      NULL);
}

/* ─── Public: data update functions ─────────────────────────────── */

void screens_set_time(int hours, int minutes, int seconds) {
  clock_screen_set_time(hours, minutes, seconds);
}

void screens_set_date(int day, int month, int year) {
  if (date_label)
    lv_label_set_text_fmt(date_label, "%02d %s %04d", day,
                          get_month_name(month), year);
}

void screens_set_steps(uint32_t steps) {
  if (label_step_count)
    lv_label_set_text_fmt(label_step_count, "%lu", (unsigned long)steps);
}

void screens_set_battery(int percent, bool charging, float voltage) {
  battery_screen_set_battery(percent, charging, voltage);
}

void screens_add_notification(const char* app, const char* message) {
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

void screens_set_ble_status(bool connected) {
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
