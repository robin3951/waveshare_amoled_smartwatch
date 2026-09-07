#include "ui.h"

#include "clock_task.h"
#include "screens.h"
#include "styles.h"

void ui_init(void) {
  styles_init();
  create_screens();
}

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
void ui_update_clock(const clock_time_t* time, const date_time_t* date) {
  screens_set_time(time);
  screens_set_date(date);
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

void ui_update_steps(uint32_t steps) { screens_set_steps(steps); }

/**
 * @brief Adds a new notification to the display
 *
 * @param app The name of the app sending the notification (e.g., "WhatsApp")
 * @param msg The message content of the notification
 */
void ui_add_notification(const char* app, const char* msg) {
  screens_add_notification(app, msg);
}

void ui_set_ble_status(bool connected) { screens_set_ble_status(connected); }
