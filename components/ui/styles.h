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

#define DOT_INDICATOR_PAD_ALL 4
#define DOT_INDICATOR_PAD_COLUMN 8
#define DOT_INACTIVE_COLOR_HEX 0x555555

#define CLOCK_DATE_LABEL_COLOR_HEX 0x888888
#define CLOCK_SHOE_PRINT_ICON_COLOR_HEX 0x008000
#define CLOCK_STEP_COUNT_LABEL_COLOR_HEX 0x888888

#define BATTERY_STATUS_LABEL_COLOR_HEX 0x00ff88
#define BATTERY_VOLTAGE_LABEL_COLOR_HEX 0x555555

#define NOTIFICATION_SCROLL_CONTENT_PAD_ALL 10
#define NOTIFICATION_SCROLL_CONTENT_PAD_ROW 10
#define NOTIFICATION_EMPTY_LABEL_COLOR_HEX 0x444444

#define NOTIFICATION_BUBBLE_RADIUS 16
#define NOTIFICATION_BUBBLE_BG_COLOR_HEX 0x1A1B2E
#define NOTIFICATION_BUBBLE_BORDER_COLOR_HEX 0x2A2B50
#define NOTIFICATION_BUBBLE_BORDER_WIDTH 1
#define NOTIFICATION_BUBBLE_PAD_ALL 14
#define NOTIFICATION_BUBBLE_PAD_ROW 8

#define NOTIFICATION_APP_LABEL_COLOR_HEX 0x00aaff
#define NOTIFICATION_MESSAGE_LABEL_COLOR_HEX 0xffffff

#define BLE_STATUS_LABEL_COLOR_HEX 0x555555
#define BLE_STATUS_CONNECTED_COLOR_HEX 0x00ff88
#define BLE_STATUS_DISCONNECTED_COLOR_HEX 0x555555

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

#ifdef __cplusplus
}
#endif

#endif /*PICOPIXEL_LVGL_UI_STYLES_H*/
