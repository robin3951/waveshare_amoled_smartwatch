/**
 * @file ui_preview.c
 * @brief LVGL Live Preview initialization
 * @note Only compiled when LVGL_LIVE_PREVIEW is defined
 */

#ifdef LVGL_LIVE_PREVIEW

#include "ui.h"

void lvgl_live_preview_init(void) {
  ui_init();

  clock_time_t time = {14, 30, 00};
  date_time_t date = {2026, 6, 18};

  ui_update_clock(&time, &date);
  ui_update_battery(75, false, 3.85f);
  ui_update_steps(1234);
  ui_set_ble_status(true);
  ui_add_notification("WhatsApp", "Hey, wie geht's?");
  ui_add_notification("Gmail", "Neue Nachricht von Max Mustermann");
}

#endif  // LVGL_LIVE_PREVIEW