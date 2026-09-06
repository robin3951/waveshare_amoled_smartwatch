/**
 * @file statusbar.h
 * @author @robin3951
 * @brief Header file for the statusbar overlay of the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-08-18
 *
 */

#define STATUSBAR_CONTAINER_HEIGHT 30
#define STATUSBAR_CONTAINER_WIDTH 380

#include "lvgl.h"  // IWYU pragma: keep

static lv_obj_t* create_battery_status_container(lv_obj_t* parent);
static lv_obj_t* create_bluetooth_status_container(lv_obj_t* parent);
static lv_obj_t* create_wifi_status_container(lv_obj_t* parent);
static lv_obj_t* create_speaker_status_container(lv_obj_t* parent);
static lv_obj_t* create_notification_status_container(lv_obj_t* parent);
void create_statusbar(void);
void statusbar_set_battery_status(void);
void statusbar_set_bluetooth_status(void);
void statusbar_set_wifi_status(void);
void statusbar_set_speaker_status(void);
void statusbar_set_notification_status(void);