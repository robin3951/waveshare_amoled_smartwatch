/**
 * @file clock_task.h
 * @author @robin3951
 * @brief Task for updating the clock display on the smartwatch.
 * @date 2026-07-07
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief FreeRTOS task for updating the clock display on the smartwatch.
 * @param arg Unused parameter.
 */
void clock_task(void* arg);