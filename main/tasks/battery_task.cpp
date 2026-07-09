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

    bsp_display_lock(0);
    ui_update_battery(percent, charging, voltage);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(BATTERY_TASK_PERIOD_MS));
  }
}