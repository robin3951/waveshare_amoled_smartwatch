// Public UI entry points
#include "ui.h"

#include "screens.h"

void ui_init(void) { create_screens(); }

void ui_tick(void) {
  // LVGL timer handles animation
}

/**
 * @brief Updates the clock display
 *
 * @param hour Hour (0-23)
 * @param min Minute (0-59)
 * @param sec Second (0-59)
 * @param day Day of the month (1-31)
 * @param month Month (1-12)
 * @param year Year
 */
void ui_update_clock(int hour, int min, int sec, int day, int month, int year) {
  screens_set_time(hour, min, sec);
  screens_set_date(day, month, year);
}

/**
 * @brief Updates the battery status display
 *
 * @param percent Battery percentage (0-100)
 * @param charging True if the device is currently charging, false otherwise
 * @param voltage Current battery voltage in volts (e.g., 3.85 for 3850mV)
 */
void ui_update_battery(int percent, bool charging, float voltage) {
  screens_set_battery(percent, charging, voltage);
}

void ui_add_notification(const char* app, const char* msg) {
  screens_add_notification(app, msg);
}

void ui_set_ble_status(bool connected) { screens_set_ble_status(connected); }

#ifdef LVGL_LIVE_PREVIEW
void lvgl_live_preview_init(void) {
  ui_init();
  ui_update_clock(14, 30, 0, 18, 6, 2026);
  ui_update_battery(75, false, 3.85f);
  ui_set_ble_status(true);
  ui_add_notification("WhatsApp", "Hey, wie geht's?");
  ui_add_notification("Gmail", "Neue Nachricht von Max Mustermann");
}
#endif
