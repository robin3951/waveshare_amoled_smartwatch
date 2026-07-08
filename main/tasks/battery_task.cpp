/**
 * @file battery_task.cpp
 * @author @robin3951
 * @brief Implementation of the battery display update task.
 * @date 2026-07-07
 */

#include "battery_task.h"

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "hardware.h"  // pmu hardware
#include "ui.h"

static const char* TAG = "battery";

void battery_task(void* arg) {
  while (true) {
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

    vTaskDelay(pdMS_TO_TICKS(BATTERY_TASK_PERIOD_MS));
  }
}