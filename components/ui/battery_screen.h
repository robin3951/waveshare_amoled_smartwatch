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

#define BATTERY_ICON_LABEL_X_OFFSET -10
#define BATTERY_ICON_LABEL_Y_OFFSET -10
#define BATTERY_ICON_LABEL_DEFAULT_PLACEHOLDER "---"

#define BATTERY_PERCENT_LABEL_Y_OFFSET -20
#define BATTERY_PERCENT_LABEL_DEFAULT_PLACEHOLDER "--%"

#define BATTERY_STATUS_LABEL_Y_OFFSET 45
#define BATTERY_STATUS_LABEL_DEFAULT_PLACEHOLDER ""

#define BATTERY_VOLTAGE_LABEL_Y_OFFSET 85
#define BATTERY_VOLTAGE_LABEL_DEFAULT_PLACEHOLDER "-.-- V"

#define BATTERY_LOW_THRESHOLD 20
#define BATTERY_MEDIUM_THRESHOLD 50

#define BATTERY_LOW_COLOR_HEX 0xff3333
#define BATTERY_MEDIUM_COLOR_HEX 0xffaa00
#define BATTERY_CHARGING_COLOR_HEX 0x00ff88

#define BATTERY_STATUS_TEXT_CHARGING "Charging"

#define BATTERY_VOLTAGE_COLOR_HEX 0x555555

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
void create_battery_tile(lv_obj_t* battery_tile);

/**
 * @brief Returns an string for the battery icon color depending on the battery
 * percentage and the charging state
 *
 * @param percent battery percentage
 * @param charging battery charging state, true if charging, otherwise false
 * @return char* representing the icon and color to display
 */
static const char* get_battery_icon(uint8_t percent, bool charging);

/**
 * @brief Changes the appearance of the battery icon and the percentage label
 * depending on the battery percentage, charging state and voltage
 *
 * @param percent battery percentage
 * @param charging battery charging state, true if charging, otherwise false
 * @param voltage battery voltage
 */
void battery_screen_set_battery(uint8_t percent, bool charging, float voltage);