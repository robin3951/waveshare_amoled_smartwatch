/**
 * @file pedometer_task.cpp
 * @author @robin3951
 * @brief Implementation of the pedometer display update task.
 * @date 2026-07-08
 */

#include "pedometer_task.h"

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "hardware.h"  // qmi hardware
#include "ui.h"

static const char* TAG = "pedometer";

void pedometer_task(void* arg) {
  while (true) {
    IMUdata acc;
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      ESP_LOGI(TAG, "Accel: x=%.2f y=%.2f z=%.2f", acc.x, acc.y, acc.z);
    } else {
      ESP_LOGE(TAG, "Accel: read failed");
    }

    bsp_display_lock(0);
    ui_update_steps(steps);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(PEDOMETER_TASK_PERIOD_MS));
  }
}