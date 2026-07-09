/**
 * @file screen_timeout_task.hpp
 * @author @robin3951
 * @brief Header file for the screen timeout task.
 * @date 2026-07-07
 */

#pragma once
#include <atomic>
#include <cstdint>

#include "freertos/FreeRTOS.h"  // IWYU pragma: keep
#include "freertos/task.h"

/** @brief FreeRTOS stack depth for the screen timeout task in bytes. */
static constexpr uint32_t SCREEN_TIMEOUT_TASK_STACK_DEPTH = 4096;

/** @brief FreeRTOS priority for the screen timeout task. */
static constexpr UBaseType_t SCREEN_TIMEOUT_TASK_PRIORITY = 2;

/** @brief Screen manager task period in milliseconds (5 Hz). */
static constexpr uint32_t SCREEN_TIMEOUT_TASK_PERIOD_MS = 200;

/**
 * @brief Current screen state, true if the screen is on, false if it is off.
 *
 * This variable is updated by screen_timeout_task at 5 Hz.
 * Thread-safe — read from any task without a mutex.
 */
extern std::atomic<bool> screen_on;

/** @brief Timeout in milliseconds after which the screen turns off. */
static constexpr uint32_t SCREEN_SLEEP_THRESHOLD_MS = 15000;

/** @brief Timeout in milliseconds after which the screen turns on. */
static constexpr uint32_t SCREEN_WAKE_THRESHOLD_MS = 500;

/**
 * @brief Task that manages the screen timeout functionality.
 *
 * This task monitors the inactivity time of the display and turns off the
 * backlight after a specified timeout period. It also turns the backlight back
 * on when user activity is detected.
 *
 * @param arg Pointer to task arguments (not used).
 */
void screen_timeout_task(void* arg);