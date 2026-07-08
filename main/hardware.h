/**
 * @file hardware.h
 * @author @robin3951
 * @brief Hardware initialization for the sensors components of the smartwatch
 * @date 2026-07-07
 */

#pragma once
// includes from components
#include "SensorPCF85063.hpp"
#include "SensorQMI8658.hpp"
#include "XPowersAXP2101.hpp"

/** @brief Global RTC instance (PCF85063), shared across all tasks. */
extern SensorPCF85063 rtc;

/** @brief Global PMU instance (AXP2101), shared across all tasks. */
extern XPowersAXP2101 pmu;

/** @brief Global IMU instance (QMI8658A), shared across all tasks. */
extern SensorQMI8658 qmi;

/**
 * @brief Initializes the hardware components of the smartwatch.
 *
 * This function initializes the RTC, PMU, and IMU sensors. It should be called
 * before starting any tasks that depend on these hardware components.
 *
 * @return true if all hardware components were initialized successfully, false
 * if any component failed — the system should not start.
 */
bool hardware_init(void);