/**
 * @file screen_timeout_task.cpp
 * @author @robin3951
 * @brief Implementation of the screen timeout task.
 * @date 2026-07-07
 */

#include "screen_timeout_task.hpp"

#include "bsp/esp-bsp.h"  // IWYU pragma: keep
#include "esp_log.h"

static const char* TAG = "screen_timeout";

std::atomic<bool> screen_on{true};

void screen_timeout_task(void* arg) {
  while (true) {
    uint32_t idle_ms = lv_display_get_inactive_time(NULL);

    bsp_display_lock(0);
    if (screen_on && idle_ms > SCREEN_SLEEP_THRESHOLD_MS) {
      bsp_display_backlight_off();
      screen_on = false;
      ESP_LOGI(TAG, "Screen turned off due to inactivity");
    } else if (!screen_on && idle_ms < SCREEN_WAKE_THRESHOLD_MS) {
      bsp_display_backlight_on();
      screen_on = true;
      ESP_LOGI(TAG, "Screen turned on due to activity");
    }
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(SCREEN_TIMEOUT_TASK_PERIOD_MS));
  }
}