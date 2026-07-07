#include "clock_task.h"

#include "bsp/esp-bsp.h"
#include "hardware.h"  // rtc hardware
#include "ui.h"

void clock_task(void* arg) {
  while (1) {
    RTC_DateTime dt = rtc.getDateTime();

    bsp_display_lock(0);
    ui_update_clock(dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getDay(),
                    dt.getMonth(), dt.getYear());
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}