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

/**
 * @brief Returns the name of the month corresponding to the given month
 * number.
 *
 * @param month The month number (1-12).
 * @return A string representing the name of the month, or "Invalid" if the
 * month number is out of range.
 */
const char* get_month_name(int month);

/**
 * @brief Sets the curent rtc time values to the labels
 *
 * @param hours current hour digits
 * @param minutes current minute digits
 * @param seconds current second digits
 */
void clock_screen_set_time(int hours, int minutes, int seconds);

/**
 * @brief
 *
 * @param day
 * @param month
 * @param year
 */
void clock_screen_set_date(int day, int month, int year);

/**
 * @brief
 *
 * @param steps
 */
void clock_screen_set_steps(uint32_t steps);
