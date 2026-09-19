/**
 * @file styles.h
 * @author @robin3951
 * @brief Header file for LVGL UI styles used in the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-07-14
 *
 */

#ifndef PICOPIXEL_LVGL_UI_STYLES_H
#define PICOPIXEL_LVGL_UI_STYLES_H

#include "fonts/fonts.h"  // IWYU pragma: keep
#include "lvgl.h"         // IWYU pragma: keep

#ifdef __cplusplus
extern "C" {
#endif

/* ===== BASIC COLOR PALETTE  ===== */

#define COLOR_CHARCOAL_HEX   0x444444
#define COLOR_DARK_GRAY_HEX  0x555555
#define COLOR_LIGHT_GRAY_HEX 0x888888

#define COLOR_MIDNIGHT_NAVY_HEX 0x1A1B2E
#define COLOR_DEEP_INDIGO_HEX   0x2A2B50

#define COLOR_GREEN_HEX    0x008000
#define COLOR_SKY_BLUE_HEX 0x00aaff
#define COLOR_BLUE_HEX     0x0000ff
#define COLOR_YELLOW_HEX   0xffdf00
#define COLOR_RED_HEX      0xff0000
#define COLOR_WHITE_HEX    0xffffff

/* ===== COLOR CONSTANTS ===== */

#define DOT_INACTIVE_COLOR_HEX COLOR_LIGHT_GRAY_HEX

#define CLOCK_DATE_LABEL_COLOR_HEX       COLOR_LIGHT_GRAY_HEX
#define CLOCK_SHOE_PRINT_ICON_COLOR_HEX  COLOR_GREEN_HEX
#define CLOCK_STEP_COUNT_LABEL_COLOR_HEX COLOR_LIGHT_GRAY_HEX

#define STATUSBAR_CHARGING_ICON_COLOR         COLOR_YELLOW_HEX
#define STATUSBAR_BLUETOOTH_ICON_COLOR        COLOR_GREEN_HEX
#define STATUSBAR_NOTIFICATION_ICON_COLOR     COLOR_YELLOW_HEX
#define STATUSBAR_NOTIFICATION_COUNT_BG_COLOR COLOR_BLUE_HEX

#define BATTERY_CHARGING_COLOR_HEX COLOR_GREEN_HEX
#define BATTERY_MEDIUM_COLOR_HEX   COLOR_YELLOW_HEX
#define BATTERY_LOW_COLOR_HEX      COLOR_RED_HEX

#define BLE_STATUS_CONNECTED_COLOR_HEX    COLOR_GREEN_HEX
#define BLE_STATUS_DISCONNECTED_COLOR_HEX COLOR_DARK_GRAY_HEX

#define NOTIFICATION_BUBBLE_BG_COLOR_HEX     COLOR_MIDNIGHT_NAVY_HEX
#define NOTIFICATION_BUBBLE_BORDER_COLOR_HEX COLOR_DEEP_INDIGO_HEX
#define NOTIFICATION_EMPTY_LABEL_COLOR_HEX   COLOR_CHARCOAL_HEX
#define NOTIFICATION_APP_LABEL_COLOR_HEX     COLOR_SKY_BLUE_HEX
#define NOTIFICATION_MESSAGE_LABEL_COLOR_HEX COLOR_WHITE_HEX

/* ===== LAYOUT CONSTANTS ===== */

#define DOT_INDICATOR_PAD_ALL    4
#define DOT_INDICATOR_PAD_COLUMN 8

#define BLUETOOTH_STATUS_CONTAINER_PAD_LEFT 20

#define NOTIFICATION_SCROLL_CONTENT_PAD_ALL 10
#define NOTIFICATION_SCROLL_CONTENT_PAD_ROW 10

#define NOTIFICATION_BUBBLE_RADIUS 16

#define NOTIFICATION_BUBBLE_BORDER_WIDTH 1

#define NOTIFICATION_BUBBLE_PAD_ALL 14
#define NOTIFICATION_BUBBLE_PAD_ROW 8

#define STATUSBAR_CONTAINER_PAD_LEFT  45
#define STATUSBAR_CONTAINER_PAD_RIGHT 5
#define STATUSBAR_CONTAINER_PAD_TOP   6

#define STATUSBAR_NOTIFICATION_COUNT_PAD_ALL 3

/**@brief Initializes LVGL styles for the clock tile and its elements */
void init_clock_styles(void);

void init_dot_indicator_styles(void);

/**@brief Initializes LVGL styles for the notification tile and its elements */
void init_notification_styles(void);

/**@brief Initializes all LVGL styles used in the UI */
void styles_init(void);

/*── Getter Functions for Shared Styles
 * ──────────────────────────────────────────────*/
lv_style_t* styles_tile_bg(void);
lv_style_t* styles_dot_container(void);
lv_style_t* styles_dot_inactive(void);
lv_style_t* styles_dot_active(void);

/*── Getter Functions for Clock Tile Styles
 * ──────────────────────────────────────────*/
lv_style_t* styles_time_hours_minutes_label(void);
lv_style_t* styles_time_seconds_label(void);
lv_style_t* styles_date_label(void);
lv_style_t* styles_shoe_print_icon(void);
lv_style_t* styles_step_count_label(void);

/*── Getter Functions for Battery Tile Styles
 * ──────────────────────────────────────────*/
lv_style_t* styles_battery_icon_label(void);
lv_style_t* styles_battery_percent_label(void);
lv_style_t* styles_battery_status_label(void);
lv_style_t* styles_battery_voltage_label(void);

/*── Getter Functions for Notification Tile Styles
 * ──────────────────────────────────────────*/
lv_style_t* styles_ble_status_label(void);
lv_style_t* styles_notification_scroll_content(void);
lv_style_t* styles_notification_empty_label(void);
lv_style_t* styles_notification_bubble(void);
lv_style_t* styles_notification_app_label(void);
lv_style_t* styles_notification_message_label(void);
lv_style_t* styles_ble_status_connected(void);
lv_style_t* styles_ble_status_disconnected(void);

/*── Getter Functions for Status Bar Styles
 * ──────────────────────────────────────────*/
lv_style_t* styles_statusbar_container(void);

lv_style_t* styles_battery_status_container(void);
lv_style_t* styles_battery_icon(void);
lv_style_t* styles_battery_charging_icon(void);
lv_style_t* styles_battery_percentage_label(void);

lv_style_t* styles_bluetooth_status_container(void);
lv_style_t* styles_bluetooth_icon(void);

lv_style_t* styles_wifi_status_container(void);
lv_style_t* styles_wifi_icon(void);
lv_style_t* styles_wifi_icon_background(void);

lv_style_t* styles_speaker_status_container(void);
lv_style_t* styles_speaker_icon(void);

lv_style_t* styles_notification_status_container(void);
lv_style_t* styles_notification_icon(void);
lv_style_t* styles_notification_count_container(void);
lv_style_t* styles_notification_count_label(void);

#ifdef __cplusplus
}
#endif

#endif /*PICOPIXEL_LVGL_UI_STYLES_H*/
