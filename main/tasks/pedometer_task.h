/**
 * @file pedometer_task.h
 * @author @robin3951
 * @brief Task for updating the pedometer display on the smartwatch.
 * @date 2026-07-08
 */

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** @brief FreeRTOS stack depth for the pedometer task in bytes. */
static constexpr uint32_t PEDOMETER_TASK_STACK_DEPTH = 4096;

/** @brief FreeRTOS priority for the pedometer task. */
static constexpr UBaseType_t PEDOMETER_TASK_PRIORITY = 3;

/** @brief Pedometer display update interval in milliseconds. */
static constexpr uint32_t PEDOMETER_TASK_PERIOD_MS = 20;  // 50 Hz

/**
 * @brief FreeRTOS task for updating the pedometer display every second.
 * @param arg Unused parameter.
 */
void pedometer_task(void* arg);