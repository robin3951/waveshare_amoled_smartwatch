// Public UI API
#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "clock_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);
void ui_tick(void);

// Clock/battery — called from FreeRTOS tasks (under bsp_display_lock)
void ui_update_clock(const clock_time_t* time, const date_time_t* date);
void ui_update_battery(int percent, bool charging, float voltage);
void ui_update_steps(uint32_t steps);

// Notifications — called from BLE task (under lvgl_port_lock)
void ui_add_notification(const char* app, const char* msg);
void ui_set_ble_status(bool connected);

#ifdef __cplusplus
}
#endif
