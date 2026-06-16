// Public UI entry points
#include "ui.h"
#include "screens.h"

void ui_init(void)
{
    create_screens();
}

void ui_tick(void)
{
    // LVGL timer handles animation
}

void ui_update_clock(int hour, int min, int sec, int day, int month, int year)
{
    screens_set_time(hour, min, sec);
    screens_set_date(day, month, year);
}

void ui_update_battery(int percent, bool charging, float voltage)
{
    screens_set_battery(percent, charging, voltage);
}

void ui_add_notification(const char *app, const char *msg)
{
    screens_add_notification(app, msg);
}

void ui_set_ble_status(bool connected)
{
    screens_set_ble_status(connected);
}
