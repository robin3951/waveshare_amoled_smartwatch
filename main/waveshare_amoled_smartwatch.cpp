#include <stdio.h>

#include "SensorPCF85063.hpp"
#include "SensorQMI8658.hpp"
#include "XPowersAXP2101.hpp"
#include "ble_chronos.h"
#include "bsp/esp-bsp.h"  // IWYU pragma: keep
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"  // IWYU pragma: keep
#include "freertos/task.h"
#include "ui.h"

static const char* TAG = "main";

static SensorPCF85063 rtc;
static XPowersAXP2101 pmu;
static SensorQMI8658 qmi;

// ─── Clock task ──────────────────────────────────────────────────────────────

static void clock_task(void* arg) {
  while (1) {
    RTC_DateTime dt = rtc.getDateTime();

    bsp_display_lock(0);
    ui_update_clock(dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getDay(),
                    dt.getMonth(), dt.getYear());
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ─── Battery / step-counter task ─────────────────────────────────────────────

static void battery_task(void* arg) {
  while (1) {
    int percent = pmu.getBatteryPercent();
    bool charging = pmu.isCharging();
    float voltage = pmu.getBattVoltage() / 1000.0f;

    IMUdata acc;
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      ESP_LOGI(TAG, "Accel: x=%.2f y=%.2f z=%.2f", acc.x, acc.y, acc.z);
    } else {
      ESP_LOGI(TAG, "Accel: read failed");
    }

    bsp_display_lock(0);
    ui_update_battery(percent, charging, voltage);
    // ui_update_steps(steps);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ─── Main ────────────────────────────────────────────────────────────────────

extern "C" void app_main(void) {
  // Suppress SPI master debug flood so IMU diagnostic output is visible in logs
  esp_log_level_set("spi_master", ESP_LOG_WARN);

  bsp_i2c_init();
  i2c_master_bus_handle_t i2c = bsp_i2c_get_handle();

  if (!rtc.begin(i2c)) {
    ESP_LOGE(TAG, "RTC init failed!");
  }

  if (!pmu.begin(i2c, AXP2101_SLAVE_ADDRESS)) {
    ESP_LOGE(TAG, "PMU init failed!");
  }

  if (!qmi.begin(i2c, QMI8658_L_SLAVE_ADDRESS)) {
    ESP_LOGE(TAG, "QMI8658 init failed!");
  } else {
    ESP_LOGI(TAG, "QMI8658 initialized");
  }

  if (qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                              SensorQMI8658::ACC_ODR_62_5Hz,
                              SensorQMI8658::LPF_OFF)) {
    ESP_LOGI(TAG, "QMI8658 accelerometer configured");
  } else {
    ESP_LOGE(TAG, "QMI8658 accelerometer configuration failed");
  }

  if (qmi.enableAccelerometer()) {
    ESP_LOGI(TAG, "QMI8658 accelerometer enabled");
  } else {
    ESP_LOGE(TAG, "QMI8658 accelerometer enable failed");
  }

  bsp_display_start();

  bsp_display_lock(0);
  ui_init();
  bsp_display_unlock();

  ble_chronos_init();

  xTaskCreate(clock_task, "clock", 4096, NULL, 5, NULL);
  xTaskCreate(battery_task, "battery", 4096, NULL, 3, NULL);

  ESP_LOGI(TAG, "Tasks started!");
}
