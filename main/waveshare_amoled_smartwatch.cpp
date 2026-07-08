/**
 * @file waveshare_amoled_smartwatch.cpp
 * @author @robin3951
 * @brief Application entry point for the Waveshare ESP32-S3 AMOLED Smartwatch
 *        firmware.
 * @date 2026-07-07
 *
 * @details
 * Orchestrates the startup sequence and launches all FreeRTOS tasks.
 *
 * ### Initialization order
 * 1. Suppress SPI debug log flood (spi_master → WARN)
 * 2. hardware_init()  — I2C bus, RTC (PCF85063), PMU (AXP2101), IMU (QMI8658A)
 * 3. bsp_display_start() — QSPI panel, LVGL port
 * 4. ui_init()        — LVGL screen setup (inside display lock)
 * 5. ble_chronos_init() — NimBLE stack, Chronos NUS service
 *
 * ### FreeRTOS tasks
 * | Task          | Priority | Stack  | Period  | Responsibility          |
 * |---------------|----------|--------|---------|-------------------------|
 * | clock_task    | 5        | 4096 B | 1 s     | RTC read → UI clock     |
 * | battery_task  | 3        | 4096 B | 1 s     | PMU read → UI battery   |
 *
 * @note app_main() must not block — FreeRTOS scheduler starts after it returns.
 */

#include "ble_chronos.h"
#include "bsp/esp-bsp.h"  // IWYU pragma: keep
#include "esp_log.h"
#include "freertos/FreeRTOS.h"  // IWYU pragma: keep
#include "freertos/task.h"
#include "hardware.h"
#include "tasks/battery_task.h"
#include "tasks/clock_task.h"
#include "ui.h"

static const char* TAG = "main";

/**
 * @brief ESP-IDF application entry point.
 *
 * Called once by the ESP-IDF startup code after FreeRTOS is initialized.
 * Returns immediately after creating tasks — the scheduler takes over.
 */
extern "C" void app_main(void) {
  // Suppress SPI master debug flood so IMU diagnostic output is visible in logs
  esp_log_level_set("spi_master", ESP_LOG_WARN);

  // Initialize hardware components (I2C, RTC, PMU, IMU)
  if (!hardware_init()) {
    ESP_LOGE(TAG, "Hardware initialization failed!");
    return;
  }

  // Start the display and initialize the UI (inside display lock)
  bsp_display_start();
  bsp_display_lock(0);
  ui_init();
  bsp_display_unlock();

  // Initialize bluetooth connection service (NimBLE stack)
  ble_chronos_init();

  // FreeRTOS tasks creation
  xTaskCreate(clock_task, "clock", CLOCK_TASK_STACK_DEPTH, NULL,
              CLOCK_TASK_PRIORITY, NULL);
  xTaskCreate(battery_task, "battery", BATTERY_TASK_STACK_DEPTH, NULL,
              BATTERY_TASK_PRIORITY, NULL);

  ESP_LOGI(TAG, "Tasks started!");
}
