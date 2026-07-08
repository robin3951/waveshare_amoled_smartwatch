/**
 * @file clock_task.h
 * @author @robin3951
 * @brief Task for updating the clock display on the smartwatch.
 * @date 2026-07-07
 */

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** @brief FreeRTOS stack depth for the clock task in bytes. */
static constexpr uint32_t CLOCK_TASK_STACK_DEPTH = 4096;

/** @brief FreeRTOS priority for the clock task. */
static constexpr UBaseType_t CLOCK_TASK_PRIORITY = 5;

/** @brief Clock display update interval in milliseconds. */
static constexpr uint32_t CLOCK_TASK_PERIOD_MS = 1000;

/**
 * @brief FreeRTOS task for updating the clock display every second.
 * @param arg Unused parameter.
 */
void clock_task(void* arg);