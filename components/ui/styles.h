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

/**@brief Basic background style for all tiles */
extern lv_style_t _style_tile_bg;

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

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Initializes all LVGL styles used in the UI */
void styles_init(void);

/**@brief Getter function for background style of all tiles
 * @return pointer to the basic tile background style */
lv_style_t* styles_tile_bg(void);

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

#ifdef __cplusplus
}
#endif

#endif /*PICOPIXEL_LVGL_UI_STYLES_H*/
