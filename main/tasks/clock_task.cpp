/**
 * @file clock_task.cpp
 * @author @robin3951
 * @brief Implementation of the clock display update task.
 * @date 2026-07-07
 */

#include "clock_task.h"

#include "bsp/esp-bsp.h"
#include "hardware.h"  // rtc hardware
#include "ui.h"

void clock_task(void* arg) {
  while (true) {
    RTC_DateTime dt = rtc.getDateTime();

    bsp_display_lock(0);
    ui_update_clock(dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getDay(),
                    dt.getMonth(), dt.getYear());
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(CLOCK_TASK_PERIOD_MS));
  }
}