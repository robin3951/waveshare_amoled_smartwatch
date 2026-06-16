// Public UI API
#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);
void ui_tick(void);

// Clock/battery — called from FreeRTOS tasks (under bsp_display_lock)
void ui_update_clock(int hour, int min, int sec, int day, int month, int year);
void ui_update_battery(int percent, bool charging, float voltage);

// Notifications — called from BLE task (under lvgl_port_lock)
void ui_add_notification(const char *app, const char *msg);
void ui_set_ble_status(bool connected);

#ifdef __cplusplus
}
#endif
