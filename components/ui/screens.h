// UI screens — 3-tile horizontal swipe navigation
#pragma once
#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UI_SCREEN_COUNT   3
#define MAX_NOTIFICATIONS 20

extern lv_obj_t *ui_tileview;
extern lv_obj_t *ui_tile_clock;
extern lv_obj_t *ui_tile_battery;
extern lv_obj_t *ui_tile_notif;

void create_screens(void);

// Clock / battery update (called from FreeRTOS tasks under LVGL lock)
void screens_set_time(int h, int m, int s);
void screens_set_date(int d, int mo, int y);
void screens_set_battery(int percent, bool charging, float voltage);

// Notification tile (called from BLE task under LVGL lock)
void screens_add_notification(const char *app, const char *msg);
void screens_set_ble_status(bool connected);

#ifdef __cplusplus
}
#endif
