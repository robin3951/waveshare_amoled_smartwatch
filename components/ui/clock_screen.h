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

#define CLOCK_TIME_HOURS_LABEL_Y_OFFSET -110

#define CLOCK_TIME_MINUTES_LABEL_Y_OFFSET 40

#define CLOCK_TIME_SECONDS_LABEL_X_OFFSET -40
#define CLOCK_TIME_SECONDS_LABEL_Y_OFFSET 40

#define CLOCK_DATE_LABEL_Y_OFFSET -110

#define ICON_SHOE_PRINT_UNICODE "\xEF\x95\x8B"
#define ICON_SHOE_PRINT_X_OFFSET -30
#define ICON_SHOE_PRINT_Y_OFFSET -50

#define STEP_COUNT_LABEL_X_OFFSET 30
#define STEP_COUNT_LABEL_Y_OFFSET -50

#define CLOCK_TIME_DEFAULT_PLACEHOLDER "--"
#define STEP_COUNT_DEFAULT_PLACEHOLDER "---"

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
const char* get_month_name(uint8_t month);

/**
 * @brief Sets the curent rtc time values to the labels
 *
 * @param hours current hour digits
 * @param minutes current minute digits
 * @param seconds current second digits
 */
void clock_screen_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

/**
 * @brief Sets the curent rtc time values to the labels
 *
 * @param day current day digits
 * @param month current month digits
 * @param year current year digits
 */
void clock_screen_set_date(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief Sets the step count value to the label
 *
 * @param steps current step count
 */
void clock_screen_set_steps(uint32_t steps);
