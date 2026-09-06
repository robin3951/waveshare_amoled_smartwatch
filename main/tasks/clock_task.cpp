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

static clock_time_t current_time = {0, 0, 0};
static date_time_t current_date = {0, 0, 0};

void clock_task(void* arg) {
  while (true) {
    RTC_DateTime date_time = rtc.getDateTime();

    current_time.hours = date_time.getHour();
    current_time.minutes = date_time.getMinute();
    current_time.seconds = date_time.getSecond();

    current_date.year = date_time.getYear();
    current_date.month = date_time.getMonth();
    current_date.day = date_time.getDay();

    bsp_display_lock(0);
    ui_update_clock(&current_time, &current_date);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(CLOCK_TASK_PERIOD_MS));
  }
}