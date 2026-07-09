/**
 * @file battery_task.h
 * @author @robin3951
 * @brief Task for updating the battery display on the smartwatch.
 * @date 2026-07-07
 */

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** @brief FreeRTOS stack depth for the battery task in bytes. */
static constexpr uint32_t BATTERY_TASK_STACK_DEPTH = 4096;

/** @brief FreeRTOS priority for the battery task. */
static constexpr UBaseType_t BATTERY_TASK_PRIORITY = 3;

/** @brief Battery display update interval in milliseconds. */
static constexpr uint32_t BATTERY_TASK_PERIOD_MS = 5000;

/**
 * @brief FreeRTOS task for updating the battery display every second.
 * @param arg Unused parameter.
 */
void battery_task(void* arg);