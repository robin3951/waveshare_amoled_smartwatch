/**
 * @file battery_task.h
 * @author @robin3951
 * @brief Task for updating the battery display on the smartwatch.
 * @date 2026-07-07
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifndef LVGL_LIVE_PREVIEW
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

typedef struct {
  uint8_t percent;
  bool charging;
  float voltage;
} battery_status_t;

/** @brief FreeRTOS stack depth for the battery task. */
#define BATTERY_TASK_STACK_DEPTH 4096

/** @brief FreeRTOS priority for the battery task. */
#define BATTERY_TASK_PRIORITY 3

/** @brief Battery display update interval in milliseconds. */
#define BATTERY_TASK_PERIOD_MS 5000

/**
 * @brief FreeRTOS task for updating the battery display every second.
 * @param arg Unused parameter.
 */
#ifndef LVGL_LIVE_PREVIEW
void battery_task(void* arg);
#endif