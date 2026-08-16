// UI screens — 3-tile horizontal swipe navigation
#pragma once
#include <stdbool.h>

#include "lvgl.h"  // IWYU pragma: keep

#ifdef __cplusplus
extern "C" {
#endif

#define UI_SCREEN_COUNT 3

#define UI_TILE_CLOCK_IDX 0
#define UI_TILE_BATTERY_IDX 1
#define UI_TILE_NOTIFICATION_IDX 2

#define DOT_ACTIVE_SIZE 10
#define DOT_INACTIVE_SIZE 8

#define DOT_INDICATOR_Y_OFFSET -14

void create_screens(void);

// Clock / battery update (called from FreeRTOS tasks under LVGL lock)
void screens_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void screens_set_date(uint8_t day, uint8_t month, uint16_t year);
void screens_set_battery(uint8_t percent, bool charging, float voltage);
void screens_set_steps(uint32_t steps);

// Notification tile (called from BLE task under LVGL lock)
void screens_add_notification(const char* app, const char* msg);
void screens_set_ble_status(bool connected);

#ifdef __cplusplus
}
#endif
