#include <stdio.h>
#include "bsp/esp-bsp.h"
#include "lvgl.h"
#include "SensorPCF85063.hpp"
#include "XPowersAXP2101.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static SensorPCF85063 rtc;
static XPowersAXP2101 pmu;

static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *bat_percent_label;
static lv_obj_t *bat_icon_label;
static lv_obj_t *charging_label;


static const char* get_battery_icon(int percent, bool charging)
{
    if (charging)      return "CHG";
    if (percent >= 90) return "100";
    if (percent >= 70) return " 75";
    if (percent >= 45) return " 50";
    if (percent >= 20) return " 25";
    return "LOW";
}

static void clock_task(void *arg)
{
    while (1) {
        RTC_DateTime dt = rtc.getDateTime();

        bsp_display_lock(0);
        lv_label_set_text_fmt(time_label, "%02d:%02d:%02d",
                              dt.getHour(),
                              dt.getMinute(),
                              dt.getSecond());
        lv_label_set_text_fmt(date_label, "%02d.%02d.%04d",
                              dt.getDay(),
                              dt.getMonth(),
                              dt.getYear());
        bsp_display_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void battery_task(void *arg)
{
    while (1) {
        int   percent  = pmu.getBatteryPercent();
        bool  charging = pmu.isCharging();
        float voltage  = pmu.getBattVoltage() / 1000.0f;

        const char *icon = get_battery_icon(percent, charging);

        lv_color_t color;
        if (charging) {
            color = lv_color_hex(0x00ff88);
        } else if (percent < 20) {
            color = lv_color_hex(0xff3333);
        } else if (percent < 50) {
            color = lv_color_hex(0xffaa00);
        } else {
            color = lv_color_hex(0xffffff);
        }

        bsp_display_lock(0);

        lv_label_set_text(bat_icon_label, icon);
        lv_obj_set_style_text_color(bat_icon_label, color, LV_PART_MAIN);

        lv_label_set_text_fmt(bat_percent_label, "%d%%", percent);
        lv_obj_set_style_text_color(bat_percent_label, color, LV_PART_MAIN);

        if (charging) {
            lv_label_set_text(charging_label, "Laden");
            lv_obj_set_style_text_color(charging_label,
                                        lv_color_hex(0x00ff88), LV_PART_MAIN);
        } else {
            lv_label_set_text_fmt(charging_label, "%.2f V", voltage);
            lv_obj_set_style_text_color(charging_label,
                                        lv_color_hex(0x555555), LV_PART_MAIN);
        }

        bsp_display_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


/* ═══════════════════════════════════════════════════════════
 * UI
 * ═══════════════════════════════════════════════════════════ */
static void create_ui(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    bat_icon_label = lv_label_create(scr);
    lv_label_set_text(bat_icon_label, "---");
    lv_obj_set_style_text_font(bat_icon_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(bat_icon_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(bat_icon_label, LV_ALIGN_TOP_RIGHT, -10, 10);

    bat_percent_label = lv_label_create(scr);
    lv_label_set_text(bat_percent_label, "--%");
    lv_obj_set_style_text_font(bat_percent_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(bat_percent_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(bat_percent_label, LV_ALIGN_TOP_RIGHT, -60, 80);

    charging_label = lv_label_create(scr);
    lv_label_set_text(charging_label, "");
    lv_obj_set_style_text_font(charging_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(charging_label, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_align(charging_label, LV_ALIGN_TOP_RIGHT, -60, 60);

    time_label = lv_label_create(scr);
    lv_label_set_text(time_label, "--:--:--");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(time_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -10);

    date_label = lv_label_create(scr);
    lv_label_set_text(date_label, "--.--.----");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 55);
}


/* ═══════════════════════════════════════════════════════════
 * app_main
 * ═══════════════════════════════════════════════════════════ */
extern "C" void app_main(void)
{
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
    create_ui();
    bsp_display_unlock();

    xTaskCreate(clock_task,   "clock",   4096, NULL, 5, NULL);
    xTaskCreate(battery_task, "battery", 4096, NULL, 3, NULL);

    printf("Tasks gestartet!\n");
}