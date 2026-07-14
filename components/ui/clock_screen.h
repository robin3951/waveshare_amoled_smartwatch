/**
 * @file clock_screen.h
 * @author @robin3951
 * @brief Header file for the clock screen UI component of the Waveshare
 * ESP32-S3 AMOLED Smartwatch firmware.
 * @date 2026-07-14
 *
 */

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

/**
 * @brief LVGL object representing the clock tile.
 *
 * This object serves as the container for all clock-related UI elements,
 * including time, date, and step count.
 */
extern lv_obj_t* ui_tile_clock;

/** @brief LVGL label for displaying hours.
 */
extern lv_obj_t* time_hours_label;

/** @brief LVGL label for displaying minutes.
 */
extern lv_obj_t* time_minutes_label;

/** @brief LVGL label for displaying seconds.
 */
extern lv_obj_t* time_seconds_label;

/** @brief LVGL label for displaying date.*/
extern lv_obj_t* date_label;

/** @brief LVGL label for displaying shoe print icon.*/
extern lv_obj_t* icon_shoe_print;

/** @brief LVGL label for displaying step count.*/
extern lv_obj_t* label_step_count;

/**
 * @brief Creates the clock tile UI component.
 *
 * This function initializes and configures the clock tile, which includes
 * labels for hours, minutes, seconds, date, a shoe print icon, and a step
 * count label. It applies appropriate styles to each element and positions
 * them within the provided clock_tile object.
 *
 * @param clock_tile The parent LVGL object where the clock tile will be
 * created.
 */
void create_clock_tile(lv_obj_t* clock_tile);