/**
 * @file battery_screen.h
 * @author @robin3951
 * @brief Header file for the battery screen UI component of the Waveshare
 * ESP32-S3 AMOLED Smartwatch firmware.
 * @date 2026-07-21
 *
 */

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

/**
 * @brief Creates the battery tile UI component.
 *
 * This function initializes and configures the battery tile, which includes
 * labels for the battery status, percent, voltage and a battery icon.
 * It applies appropriate styles to each element and positions
 * them within the provided battery_tile object.
 *
 * @param battery_tile The parent LVGL object where the battery tile will be
 * created.
 */
static void create_battery_tile(lv_obj_t* battery_tile);

/**
 * @brief Returns an string for the battery icon color depending on the battery
 * percentage and the charging state
 *
 * @param percent battery percentage
 * @param charging battery charging state, true if charging, otherwise false
 * @return char* representing the icon and color to display
 */
static const char* get_battery_icon(int percent, bool charging);

/**
 * @brief Changes the appearance of the battery icon and the percentage label
 * depending on the battery percentage, charging state and voltage
 *
 * @param percent battery percentage
 * @param charging battery charging state, true if charging, otherwise false
 * @param voltage battery voltage
 */
void battery_screen_set_battery(int percent, bool charging, float voltage);