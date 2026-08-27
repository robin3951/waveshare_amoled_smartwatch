/**
 * @file styles.c
 * @author @robin3951
 * @brief Implementation of LVGL UI styles used in the Waveshare ESP32-S3 AMOLED
 * Smartwatch firmware.
 * @date 2026-07-14
 *
 */
#include "styles.h"

#include "fonts/fonts.h"

// styles shared across all tiles
static lv_style_t _style_tile_bg = {0};

static lv_style_t _style_dot_container = {0};
static lv_style_t _style_dot_inactive = {0};
static lv_style_t _style_dot_active = {0};

// styles specific to clock tile
static lv_style_t _style_time_hours_minutes_label = {0};
static lv_style_t _style_time_seconds_label = {0};
static lv_style_t _style_date_label = {0};
static lv_style_t _style_shoe_print_icon = {0};
static lv_style_t _style_step_count_label = {0};

// styles specific to battery tile
static lv_style_t _style_battery_icon_label = {0};
static lv_style_t _style_battery_percent_label = {0};
static lv_style_t _style_battery_status_label = {0};
static lv_style_t _style_battery_voltage_label = {0};

// styles specific to notification tile
static lv_style_t _style_ble_status_label = {0};
static lv_style_t _style_notification_scroll_content = {0};
static lv_style_t _style_notification_empty_label = {0};
static lv_style_t _style_notification_bubble = {0};
static lv_style_t _style_notification_app_label = {0};
static lv_style_t _style_notification_message_label = {0};
static lv_style_t _style_ble_status_connected = {0};
static lv_style_t _style_ble_status_disconnected = {0};

// styles specific to statusbar overlay
static lv_style_t _style_statusbar_container = {0};

static lv_style_t _style_battery_status_container = {0};
static lv_style_t _style_battery_icon = {0};
static lv_style_t _style_battery_charging_icon = {0};
static lv_style_t _style_battery_percentage_label = {0};

static lv_style_t _style_bluetooth_status_container = {0};
static lv_style_t _style_bluetooth_icon = {0};

static lv_style_t _style_wifi_status_container = {0};
static lv_style_t _style_wifi_icon = {0};

static lv_style_t _style_speaker_status_container = {0};
static lv_style_t _style_speaker_icon = {0};

static lv_style_t _style_notification_status_container = {0};
static lv_style_t _style_notification_icon = {0};
static lv_style_t _style_notification_count_container = {0};
static lv_style_t _style_notification_count_label = {0};

void init_shared_styles(void) {
  lv_style_init(&_style_tile_bg);
  lv_style_set_bg_color(&_style_tile_bg, lv_color_black());
  lv_style_set_bg_opa(&_style_tile_bg, LV_OPA_COVER);
  lv_style_set_border_width(&_style_tile_bg, 0);
  lv_style_set_pad_all(&_style_tile_bg, 0);
}

void init_dot_indicator_styles(void) {
  lv_style_init(&_style_dot_container);
  lv_style_set_bg_opa(&_style_dot_container, LV_OPA_TRANSP);
  lv_style_set_border_width(&_style_dot_container, 0);
  lv_style_set_pad_all(&_style_dot_container, DOT_INDICATOR_PAD_ALL);
  lv_style_set_pad_column(&_style_dot_container, DOT_INDICATOR_PAD_COLUMN);

  lv_style_init(&_style_dot_inactive);
  lv_style_set_radius(&_style_dot_inactive, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&_style_dot_inactive,
                        lv_color_hex(DOT_INACTIVE_COLOR_HEX));
  lv_style_set_bg_opa(&_style_dot_inactive, LV_OPA_COVER);
  lv_style_set_border_width(&_style_dot_inactive, 0);
  lv_style_set_pad_all(&_style_dot_inactive, 0);

  lv_style_init(&_style_dot_active);
  lv_style_set_radius(&_style_dot_active, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&_style_dot_active, lv_color_white());
  lv_style_set_bg_opa(&_style_dot_active, LV_OPA_COVER);
  lv_style_set_border_width(&_style_dot_active, 0);
  lv_style_set_pad_all(&_style_dot_active, 0);
}

void init_clock_styles(void) {
  lv_style_init(&_style_time_hours_minutes_label);
  lv_style_set_text_font(&_style_time_hours_minutes_label,
                         &lv_font_montserrat_164);
  lv_style_set_text_color(&_style_time_hours_minutes_label, lv_color_white());

  lv_style_init(&_style_time_seconds_label);
  lv_style_set_text_font(&_style_time_seconds_label, &lv_font_montserrat_48);
  lv_style_set_text_color(&_style_time_seconds_label, lv_color_white());

  lv_style_init(&_style_date_label);
  lv_style_set_text_font(&_style_date_label, &lv_font_montserrat_32);
  lv_style_set_text_color(&_style_date_label,
                          lv_color_hex(CLOCK_DATE_LABEL_COLOR_HEX));

  lv_style_init(&_style_shoe_print_icon);
  lv_style_set_text_font(&_style_shoe_print_icon, &lv_font_shoe_print_48);
  lv_style_set_text_color(&_style_shoe_print_icon,
                          lv_color_hex(CLOCK_SHOE_PRINT_ICON_COLOR_HEX));

  lv_style_init(&_style_step_count_label);
  lv_style_set_text_font(&_style_step_count_label, &lv_font_montserrat_32);
  lv_style_set_text_color(&_style_step_count_label,
                          lv_color_hex(CLOCK_STEP_COUNT_LABEL_COLOR_HEX));
}

void init_battery_styles(void) {
  lv_style_init(&_style_battery_icon_label);
  lv_style_set_text_font(&_style_battery_icon_label, &lv_font_montserrat_12);
  lv_style_set_text_color(&_style_battery_icon_label, lv_color_white());

  lv_style_init(&_style_battery_percent_label);
  lv_style_set_text_font(&_style_battery_percent_label, &lv_font_montserrat_48);
  lv_style_set_text_color(&_style_battery_percent_label, lv_color_white());

  lv_style_init(&_style_battery_status_label);
  lv_style_set_text_font(&_style_battery_status_label, &lv_font_montserrat_16);
  lv_style_set_text_color(&_style_battery_status_label,
                          lv_color_hex(BATTERY_STATUS_LABEL_COLOR_HEX));

  lv_style_init(&_style_battery_voltage_label);
  lv_style_set_text_font(&_style_battery_voltage_label, &lv_font_montserrat_20);
  lv_style_set_text_color(&_style_battery_voltage_label,
                          lv_color_hex(BATTERY_VOLTAGE_LABEL_COLOR_HEX));
}

void init_notification_styles(void) {
  lv_style_init(&_style_notification_scroll_content);
  lv_style_set_bg_color(&_style_notification_scroll_content, lv_color_black());
  lv_style_set_bg_opa(&_style_notification_scroll_content, LV_OPA_COVER);
  lv_style_set_border_width(&_style_notification_scroll_content, 0);
  lv_style_set_pad_all(&_style_notification_scroll_content,
                       NOTIFICATION_SCROLL_CONTENT_PAD_ALL);
  lv_style_set_pad_row(&_style_notification_scroll_content,
                       NOTIFICATION_SCROLL_CONTENT_PAD_ROW);

  lv_style_init(&_style_notification_empty_label);
  lv_style_set_text_font(&_style_notification_empty_label,
                         &lv_font_montserrat_16);
  lv_style_set_text_color(&_style_notification_empty_label,
                          lv_color_hex(NOTIFICATION_EMPTY_LABEL_COLOR_HEX));

  lv_style_init(&_style_notification_bubble);
  lv_style_set_radius(&_style_notification_bubble, NOTIFICATION_BUBBLE_RADIUS);
  lv_style_set_bg_color(&_style_notification_bubble,
                        lv_color_hex(NOTIFICATION_BUBBLE_BG_COLOR_HEX));
  lv_style_set_bg_opa(&_style_notification_bubble, LV_OPA_COVER);
  lv_style_set_border_color(&_style_notification_bubble,
                            lv_color_hex(NOTIFICATION_BUBBLE_BORDER_COLOR_HEX));
  lv_style_set_border_width(&_style_notification_bubble,
                            NOTIFICATION_BUBBLE_BORDER_WIDTH);
  lv_style_set_pad_all(&_style_notification_bubble,
                       NOTIFICATION_BUBBLE_PAD_ALL);
  lv_style_set_pad_row(&_style_notification_bubble,
                       NOTIFICATION_BUBBLE_PAD_ROW);

  lv_style_init(&_style_notification_app_label);
  lv_style_set_text_font(&_style_notification_app_label,
                         &lv_font_montserrat_14);
  lv_style_set_text_color(&_style_notification_app_label,
                          lv_color_hex(NOTIFICATION_APP_LABEL_COLOR_HEX));

  lv_style_init(&_style_notification_message_label);
  lv_style_set_text_font(&_style_notification_message_label,
                         &lv_font_montserrat_16_ext);
  lv_style_set_text_color(&_style_notification_message_label,
                          lv_color_hex(NOTIFICATION_MESSAGE_LABEL_COLOR_HEX));

  lv_style_init(&_style_ble_status_label);
  lv_style_set_text_font(&_style_ble_status_label, &lv_font_montserrat_12);
  lv_style_set_text_color(&_style_ble_status_label,
                          lv_color_hex(BLE_STATUS_LABEL_COLOR_HEX));

  lv_style_init(&_style_ble_status_connected);
  lv_style_set_text_color(&_style_ble_status_connected,
                          lv_color_hex(BLE_STATUS_CONNECTED_COLOR_HEX));

  lv_style_init(&_style_ble_status_disconnected);
  lv_style_set_text_color(&_style_ble_status_disconnected,
                          lv_color_hex(BLE_STATUS_DISCONNECTED_COLOR_HEX));
}

void init_statusbar_styles(void) {
  lv_style_init(&_style_statusbar_container);
  lv_style_set_border_width(&_style_statusbar_container, 0);
  lv_style_set_bg_color(&_style_statusbar_container, lv_color_black());
  lv_style_set_bg_opa(&_style_statusbar_container, LV_OPA_TRANSP);
  lv_style_set_pad_left(&_style_statusbar_container,
                        STATUSBAR_CONTAINER_PAD_LEFT);
  lv_style_set_pad_right(&_style_statusbar_container,
                         STATUSBAR_CONTAINER_PAD_RIGHT);
  lv_style_set_pad_ver(&_style_statusbar_container, 0);

  lv_style_init(&_style_battery_status_container);
  lv_style_set_border_width(&_style_battery_status_container, 0);
  lv_style_set_bg_color(&_style_battery_status_container, lv_color_black());
  lv_style_set_bg_opa(&_style_battery_status_container, LV_OPA_TRANSP);
  lv_style_set_pad_all(&_style_battery_status_container, 0);

  lv_style_init(&_style_battery_icon);
  lv_style_set_text_color(&_style_battery_icon, lv_color_white());

  lv_style_init(&_style_battery_charging_icon);
  lv_style_set_text_color(&_style_battery_charging_icon,
                          lv_color_hex(STATUSBAR_CHARGING_ICON_COLOR));

  lv_style_init(&_style_battery_percentage_label);
  lv_style_set_text_color(&_style_battery_percentage_label, lv_color_white());
  lv_style_set_text_font(&_style_battery_percentage_label,
                         &lv_font_montserrat_12);

  lv_style_init(&_style_bluetooth_status_container);
  lv_style_set_border_width(&_style_bluetooth_status_container, 0);
  lv_style_set_bg_color(&_style_bluetooth_status_container, lv_color_black());
  lv_style_set_bg_opa(&_style_bluetooth_status_container, LV_OPA_TRANSP);
  lv_style_set_pad_all(&_style_bluetooth_status_container, 0);

  lv_style_init(&_style_bluetooth_icon);
  lv_style_set_text_color(&_style_bluetooth_icon,
                          lv_color_hex(STATUSBAR_BLUETOOTH_ICON_COLOR));

  lv_style_init(&_style_wifi_status_container);
  lv_style_set_border_width(&_style_wifi_status_container, 0);
  lv_style_set_bg_color(&_style_wifi_status_container, lv_color_black());
  lv_style_set_bg_opa(&_style_wifi_status_container, LV_OPA_TRANSP);

  lv_style_init(&_style_wifi_icon);
  lv_style_set_text_color(&_style_wifi_icon, lv_color_white());

  lv_style_init(&_style_speaker_status_container);
  lv_style_set_border_width(&_style_speaker_status_container, 0);
  lv_style_set_bg_color(&_style_speaker_status_container, lv_color_black());
  lv_style_set_bg_opa(&_style_speaker_status_container, LV_OPA_TRANSP);

  lv_style_init(&_style_speaker_icon);
  lv_style_set_text_color(&_style_speaker_icon, lv_color_white());

  lv_style_init(&_style_notification_status_container);
  lv_style_set_border_width(&_style_notification_status_container, 0);
  lv_style_set_bg_color(&_style_notification_status_container,
                        lv_color_black());
  lv_style_set_bg_opa(&_style_notification_status_container, LV_OPA_TRANSP);

  lv_style_init(&_style_notification_icon);
  lv_style_set_text_color(&_style_notification_icon,
                          lv_color_hex(STATUSBAR_NOTIFICATION_ICON_COLOR));

  lv_style_init(&_style_notification_count_container);
  lv_style_set_radius(&_style_notification_count_container, LV_RADIUS_CIRCLE);
  lv_style_set_border_width(&_style_notification_count_container, 0);
  lv_style_set_bg_color(&_style_notification_count_container,
                        lv_color_hex(STATUSBAR_NOTIFICATION_COUNT_BG_COLOR));
  lv_style_set_bg_opa(&_style_notification_count_container, LV_OPA_COVER);
  lv_style_set_pad_all(&_style_notification_count_container,
                       STATUSBAR_NOTIFICATION_COUNT_PAD_ALL);

  lv_style_init(&_style_notification_count_label);
  lv_style_set_text_color(&_style_notification_count_label, lv_color_white());
  lv_style_set_text_font(&_style_notification_count_label,
                         &lv_font_montserrat_12);
  lv_style_set_border_width(&_style_notification_count_label, 0);
}

void styles_init(void) {
  init_shared_styles();
  init_dot_indicator_styles();
  init_clock_styles();
  init_battery_styles();
  init_notification_styles();
  init_statusbar_styles();
}

lv_style_t* styles_tile_bg(void) { return &_style_tile_bg; }

lv_style_t* styles_dot_container(void) { return &_style_dot_container; }

lv_style_t* styles_dot_inactive(void) { return &_style_dot_inactive; }

lv_style_t* styles_dot_active(void) { return &_style_dot_active; }

lv_style_t* styles_time_hours_minutes_label(void) {
  return &_style_time_hours_minutes_label;
}

lv_style_t* styles_time_seconds_label(void) {
  return &_style_time_seconds_label;
}

lv_style_t* styles_date_label(void) { return &_style_date_label; }

lv_style_t* styles_shoe_print_icon(void) { return &_style_shoe_print_icon; }

lv_style_t* styles_step_count_label(void) { return &_style_step_count_label; }

lv_style_t* styles_battery_icon_label(void) {
  return &_style_battery_icon_label;
}

lv_style_t* styles_battery_percent_label(void) {
  return &_style_battery_percent_label;
}

lv_style_t* styles_battery_status_label(void) {
  return &_style_battery_status_label;
}

lv_style_t* styles_battery_voltage_label(void) {
  return &_style_battery_voltage_label;
}

lv_style_t* styles_ble_status_label(void) { return &_style_ble_status_label; }

lv_style_t* styles_notification_scroll_content(void) {
  return &_style_notification_scroll_content;
}

lv_style_t* styles_notification_empty_label(void) {
  return &_style_notification_empty_label;
}

lv_style_t* styles_notification_bubble(void) {
  return &_style_notification_bubble;
}

lv_style_t* styles_notification_app_label(void) {
  return &_style_notification_app_label;
}

lv_style_t* styles_notification_message_label(void) {
  return &_style_notification_message_label;
}

lv_style_t* styles_ble_status_connected(void) {
  return &_style_ble_status_connected;
}

lv_style_t* styles_ble_status_disconnected(void) {
  return &_style_ble_status_disconnected;
}

lv_style_t* styles_statusbar_container(void) {
  return &_style_statusbar_container;
}

lv_style_t* styles_battery_status_container(void) {
  return &_style_battery_status_container;
}

lv_style_t* styles_battery_icon(void) { return &_style_battery_icon; }

lv_style_t* styles_battery_charging_icon(void) {
  return &_style_battery_charging_icon;
}

lv_style_t* styles_battery_percentage_label(void) {
  return &_style_battery_percentage_label;
}

lv_style_t* styles_bluetooth_status_container(void) {
  return &_style_bluetooth_status_container;
}

lv_style_t* styles_bluetooth_icon(void) { return &_style_bluetooth_icon; }

lv_style_t* styles_wifi_status_container(void) {
  return &_style_wifi_status_container;
}

lv_style_t* styles_wifi_icon(void) { return &_style_wifi_icon; }

lv_style_t* styles_speaker_status_container(void) {
  return &_style_speaker_status_container;
}

lv_style_t* styles_speaker_icon(void) { return &_style_speaker_icon; }

lv_style_t* styles_notification_status_container(void) {
  return &_style_notification_status_container;
}

lv_style_t* styles_notification_icon(void) { return &_style_notification_icon; }

lv_style_t* styles_notification_count_container(void) {
  return &_style_notification_count_container;
}

lv_style_t* styles_notification_count_label(void) {
  return &_style_notification_count_label;
}
