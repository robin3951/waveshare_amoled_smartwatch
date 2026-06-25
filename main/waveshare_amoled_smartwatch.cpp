#include <stdio.h>

#include "SensorPCF85063.hpp"
#include "XPowersAXP2101.hpp"
#include "ble_chronos.h"
#include "bsp/esp-bsp.h"        // IWYU pragma: keep
#include "freertos/FreeRTOS.h"  // IWYU pragma: keep
#include "freertos/task.h"
#include "ui.h"

static SensorPCF85063 rtc;
static XPowersAXP2101 pmu;

static void clock_task(void* arg) {
  while (1) {
    RTC_DateTime dt = rtc.getDateTime();

    bsp_display_lock(0);
    ui_update_clock(dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getDay(),
                    dt.getMonth(), dt.getYear());
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

static void battery_task(void* arg) {
  while (1) {
    int percent = pmu.getBatteryPercent();
    bool charging = pmu.isCharging();
    float voltage = pmu.getBattVoltage() / 1000.0f;

    bsp_display_lock(0);
    ui_update_battery(percent, charging, voltage);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

extern "C" void app_main(void) {
  bsp_i2c_init();

  i2c_master_bus_handle_t i2c = bsp_i2c_get_handle();

  if (!rtc.begin(i2c)) {
    printf("RTC init fehlgeschlagen!\n");
  }

  if (!pmu.begin(i2c, AXP2101_SLAVE_ADDRESS)) {
    printf("PMIC init fehlgeschlagen!\n");
  }

  bsp_display_start();

  bsp_display_lock(0);
  ui_init();
  bsp_display_unlock();

  ble_chronos_init();

  xTaskCreate(clock_task, "clock", 4096, NULL, 5, NULL);
  xTaskCreate(battery_task, "battery", 4096, NULL, 3, NULL);

  printf("Tasks gestartet!\n");
}
