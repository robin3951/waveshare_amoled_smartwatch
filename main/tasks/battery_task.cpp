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

static battery_status_t current_battery_status = {0, false, 0.0f};

void battery_task(void* arg) {
  while (true) {
    current_battery_status.percent = pmu.getBatteryPercent();
    current_battery_status.charging = pmu.isCharging();
    current_battery_status.voltage = pmu.getBattVoltage() / 1000.0f;

    bsp_display_lock(0);
    ui_update_battery(&current_battery_status);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(BATTERY_TASK_PERIOD_MS));
  }
}