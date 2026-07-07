/**
 * @file hardware.h
 * @author @robin3951
 * @brief hardware initialization for the sensors components of the smartwatch
 * @date 2026-07-07
 */

#pragma once
// includes from components
#include "SensorPCF85063.hpp"
#include "SensorQMI8658.hpp"
#include "XPowersAXP2101.hpp"

extern SensorPCF85063 rtc;
extern XPowersAXP2101 pmu;
extern SensorQMI8658 qmi;

/**
 * @brief Initializes the hardware components of the smartwatch.
 *
 * This function initializes the RTC, PMU, and IMU sensors. It should be called
 * before starting any tasks that depend on these hardware components.
 *
 * @return true if all hardware components were initialized successfully, false
 * otherwise.
 */
bool hardware_init(void);