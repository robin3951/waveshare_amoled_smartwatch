/**
 * @file clock_task.h
 * @author @robin3951
 * @brief Task for updating the clock display on the smartwatch.
 * @date 2026-07-07
 */

#pragma once
#include <stdint.h>

#ifndef LVGL_LIVE_PREVIEW
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} clock_time_t;

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t day;
} date_time_t;

/** @brief FreeRTOS stack depth for the clock task in bytes. */
#define CLOCK_TASK_STACK_DEPTH 4096

/** @brief FreeRTOS priority for the clock task. */
#define CLOCK_TASK_PRIORITY 5

/** @brief Clock display update interval in milliseconds. */
#define CLOCK_TASK_PERIOD_MS 1000

/**
 * @brief FreeRTOS task for updating the clock display every second.
 * @param arg Unused parameter.
 */
#ifndef LVGL_LIVE_PREVIEW
void clock_task(void* arg);
#endif

#ifdef __cplusplus
}
#endif