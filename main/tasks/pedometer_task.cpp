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
#include "step_counter.hpp"
#include "ui.h"

static const char* TAG = "pedometer";

std::atomic<uint32_t> g_step_count{0};

uint32_t pedometer_get_steps(void) { return g_step_count.load(); }
void pedometer_reset_steps(void) { g_step_count.store(0); }

void pedometer_task(void* arg) {
  StepCounter counter;
  uint32_t ui_tick = 0;

  while (true) {
    float ax = 0.0f, ay = 0.0f, az = 0.0f;

    if (qmi.getAccelerometer(ax, ay, az)) {
      counter.update(ax, ay, az);
      g_step_count.store(counter.getSteps());
    } else {
      ESP_LOGE(TAG, "Accel: read failed");
    }

    // Update UI once per second (every 50 samples × 20 ms)
    if (++ui_tick >= 50) {
      ui_tick = 0;
      bsp_display_lock(0);
      ui_update_steps(g_step_count.load());
      bsp_display_unlock();
    }

    vTaskDelay(pdMS_TO_TICKS(PEDOMETER_TASK_PERIOD_MS));
  }
}