/**
 * @file hardware.cpp
 * @brief Hardware initialization and management for the Waveshare AMOLED
 * Smartwatch.
 *
 * This file contains the implementation of the hardware initialization
 * function, which sets up the I2C bus and initializes the RTC, PMU, and IMU
 * sensors.
 */
#include "hardware.h"

#include "bsp/esp-bsp.h"  // IWYU pragma: keep
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char* TAG = "hardware";

SensorPCF85063 rtc;
XPowersAXP2101 pmu;
SensorQMI8658 qmi;

bool hardware_init(void) {
  bsp_i2c_init();
  i2c_master_bus_handle_t i2c = bsp_i2c_get_handle();

  if (!rtc.begin(i2c)) {
    ESP_LOGE(TAG, "RTC init failed!");
    return false;
  }
  ESP_LOGI(TAG, "RTC initialized");

  if (!pmu.begin(i2c, AXP2101_SLAVE_ADDRESS)) {
    ESP_LOGE(TAG, "PMU init failed!");
    return false;
  }
  ESP_LOGI(TAG, "PMU initialized");

  if (!qmi.begin(i2c, QMI8658_L_SLAVE_ADDRESS)) {
    ESP_LOGE(TAG, "QMI8658 init failed!");
    return false;
  }
  ESP_LOGI(TAG, "QMI8658 initialized");

  if (!qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                               SensorQMI8658::ACC_ODR_62_5Hz,
                               SensorQMI8658::LPF_OFF)) {
    ESP_LOGE(TAG, "QMI8658 accelerometer configuration failed");
    return false;
  }
  ESP_LOGI(TAG, "QMI8658 accelerometer configured");

  if (!qmi.enableAccelerometer()) {
    ESP_LOGE(TAG, "QMI8658 accelerometer enable failed");
    return false;
  }
  ESP_LOGI(TAG, "QMI8658 accelerometer enabled");

  ESP_LOGI(TAG, "Hardware init successful");
  return true;
}