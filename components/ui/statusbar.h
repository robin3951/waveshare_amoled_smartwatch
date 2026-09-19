/**
 * @file statusbar.h
 * @author @robin3951
 * @brief Header file for the statusbar overlay of the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-08-18
 *
 */

#define WIFI_SIGNAL_OFF    0
#define WIFI_SIGNAL_LOW    1
#define WIFI_SIGNAL_MEDIUM 2
#define WIFI_SIGNAL_HIGH   3

/* wifi symbol fonts from material symbols (lv_font_wifi_symbol_28) */

#define WIFI_SYMBOL_1_BAR "\uE4CA"  // codepoint e4ca
#define WIFI_SYMBOL_2_BAR "\uE4D9"  // codepoint e4d9
#define WIFI_SYMBOL_3_BAR "\uE63E"  // codepoint e63e
#define WIFI_SYMBOL_OFF   "\uE648"  // codepoint e648

#define STATUSBAR_CONTAINER_HEIGHT 30
#define STATUSBAR_CONTAINER_WIDTH  380

#define BATTERY_FULL_THRESHOLD   90
#define BATTERY_HIGH_THRESHOLD   70
#define BATTERY_MEDIUM_THRESHOLD 50
#define BATTERY_LOW_THRESHOLD    20

#include "battery_task.h"
#include "lvgl.h"  // IWYU pragma: keep

static lv_obj_t* create_battery_status_container(lv_obj_t* parent);
static lv_obj_t* create_bluetooth_status_container(lv_obj_t* parent);
static lv_obj_t* create_wifi_status_container(lv_obj_t* parent);
static lv_obj_t* create_speaker_status_container(lv_obj_t* parent);
static lv_obj_t* create_notification_status_container(lv_obj_t* parent);

static const char* get_battery_icon(const battery_status_t* battery_status);
static lv_color_t get_battery_color(const battery_status_t* battery_status);
const char* get_wifi_symbol(uint8_t wifi_signal_strength);

void create_statusbar(void);
void statusbar_set_battery_status(const battery_status_t* battery_status);
void statusbar_set_bluetooth_status(bool connected);
void statusbar_set_wifi_status(uint8_t wifi_signal_strength);
void statusbar_set_speaker_status(uint8_t speaker_volume);
void statusbar_set_notification_status(void);