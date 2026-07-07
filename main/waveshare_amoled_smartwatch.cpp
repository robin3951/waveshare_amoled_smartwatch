#include <stdio.h>

#include "ble_chronos.h"
#include "bsp/esp-bsp.h"  // IWYU pragma: keep
#include "esp_log.h"
#include "freertos/FreeRTOS.h"  // IWYU pragma: keep
#include "freertos/task.h"
#include "hardware.h"
#include "tasks/clock_task.h"
#include "ui.h"

static const char* TAG = "main";

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

  if (!hardware_init()) {
    ESP_LOGE(TAG, "Hardware initialization failed!");
    return;
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
