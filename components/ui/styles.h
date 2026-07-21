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

/*── Shared Styles ──────────────────────────────────────────────*/

/**@brief Basic background style for all tiles */
extern lv_style_t _style_tile_bg;

extern lv_style_t _style_dot_container;
extern lv_style_t _style_dot_inactive;
extern lv_style_t _style_dot_active;

/*── Clock Tile Styles ──────────────────────────────────────────*/

/**@brief Style for hours and minutes labels on clock tile */
extern lv_style_t _style_time_hours_minutes_label;

/**@brief Style for seconds label on clock tile */
extern lv_style_t _style_time_seconds_label;

/**@brief Style for date label on clock tile */
extern lv_style_t _style_date_label;

/**@brief Style for shoe print icon on clock tile */
extern lv_style_t _style_shoe_print_icon;

/**@brief Style for step count label on clock tile */
extern lv_style_t _style_step_count_label;

extern lv_style_t _style_battery_icon_label;
extern lv_style_t _style_battery_percent_label;
extern lv_style_t _style_battery_status_label;
extern lv_style_t _style_battery_voltage_label;

/*── Notification Tile Styles ───────────────────────────────────*/

/**@brief Style for BLE status label on notification tile */
extern lv_style_t _style_ble_status_label;

/**@brief Style for scrollable content container on notification tile */
extern lv_style_t _style_notification_scroll_content;

/**@brief Style for "no notifications" placeholder label on notification tile */
extern lv_style_t _style_notification_empty_label;

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Initializes LVGL styles for the clock tile and its elements */
void init_clock_styles(void);

void init_dot_indicator_styles(void);

/**@brief Initializes LVGL styles for the notification tile and its elements */
void init_notification_styles(void);

/**@brief Initializes all LVGL styles used in the UI */
void styles_init(void);

/*── Getter Functions for Shared Styles
 * ──────────────────────────────────────────────*/

/**@brief Getter function for background style of all tiles
 * @return pointer to the basic tile background style */
lv_style_t* styles_tile_bg(void);

lv_style_t* styles_dot_container(void);
lv_style_t* styles_dot_inactive(void);
lv_style_t* styles_dot_active(void);

/*── Getter Functions for Clock Tile Styles
 * ──────────────────────────────────────────*/

/**@brief Getter function for hours and minutes label style
 * @return pointer to the hours and minutes label style */
lv_style_t* styles_time_hours_minutes_label(void);

/**@brief Getter function for seconds label style
 * @return pointer to the seconds label style */
lv_style_t* styles_time_seconds_label(void);

/**@brief Getter function for date label style
 * @return pointer to the date label style */
lv_style_t* styles_date_label(void);

/**@brief Getter function for shoe print icon style
 * @return pointer to the shoe print icon style */
lv_style_t* styles_shoe_print_icon(void);

/**@brief Getter function for step count label style
 * @return pointer to the step count label style */
lv_style_t* styles_step_count_label(void);

lv_style_t* styles_battery_icon_label(void);
lv_style_t* styles_battery_percent_label(void);
lv_style_t* styles_battery_status_label(void);
lv_style_t* styles_battery_voltage_label(void);

/*── Getter Functions for Notification Tile Styles
 * ──────────────────────────────────────────*/

/**@brief Getter function for BLE status label style
 * @return pointer to the BLE status label style */
lv_style_t* styles_ble_status_label(void);

/**@brief Getter function for scrollable content container style
 * @return pointer to the scrollable content container style */
lv_style_t* styles_notification_scroll_content(void);

/**@brief Getter function for "no notifications" placeholder label style
 * @return pointer to the "no notifications" placeholder label style */
lv_style_t* styles_notification_empty_label(void);

#ifdef __cplusplus
}
#endif

#endif /*PICOPIXEL_LVGL_UI_STYLES_H*/
