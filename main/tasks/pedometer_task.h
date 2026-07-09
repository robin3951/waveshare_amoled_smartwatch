/**
 * @file pedometer_task.h
 * @author @robin3951
 * @brief FreeRTOS task that runs the software pedometer at 50 Hz.
 * @date 2026-07-09
 *
 * Samples the QMI8658A accelerometer at 50 Hz, feeds each sample into a
 * StepCounter pipeline, and updates the LVGL display once per second.
 *
 * The step count is exposed as a thread-safe atomic so other tasks (BLE
 * reporting, sleep tracking) can read it without a mutex:
 * @code
 *   #include "tasks/pedometer_task.h"
 *   uint32_t steps = pedometer_get_steps();
 * @endcode
 *
 * @see docs/diagrams/step_counter_class.drawio    — class structure
 * @see docs/diagrams/step_counter_pipeline.drawio — signal processing pipeline
 * @see docs/diagrams/step_counter_states.drawio   — peak detection state machine
 * @see docs/diagrams/step_counter_signal.drawio   — annotated signal trace
 */

#pragma once
#include <atomic>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** @brief FreeRTOS stack depth for the pedometer task in bytes. */
static constexpr uint32_t PEDOMETER_TASK_STACK_DEPTH = 4096;

/** @brief FreeRTOS priority for the pedometer task. */
static constexpr UBaseType_t PEDOMETER_TASK_PRIORITY = 3;

/** @brief Accelerometer sampling period in milliseconds (50 Hz). */
static constexpr uint32_t PEDOMETER_TASK_PERIOD_MS = 20;

/**
 * @brief Cumulative step count, updated by pedometer_task at 50 Hz.
 *
 * Thread-safe — read from any task without a mutex.
 */
extern std::atomic<uint32_t> g_step_count;

/**
 * @brief Returns the current cumulative step count.
 * @return Step count since last pedometer_reset_steps() or device boot.
 */
uint32_t pedometer_get_steps(void);

/**
 * @brief Resets the step count to zero.
 *
 * Intended for daily rollover (call once at midnight from an RTC event).
 * Thread-safe.
 */
void pedometer_reset_steps(void);

/**
 * @brief FreeRTOS task: samples accelerometer at 50 Hz, runs StepCounter
 *        pipeline, updates UI once per second.
 * @param arg Unused.
 */
void pedometer_task(void* arg);