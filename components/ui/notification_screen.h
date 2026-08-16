/**
 * @file notification_screen.h
 * @author @robin3951
 * @brief Header file for the notification screen of the Waveshare ESP32-S3
 * AMOLED Smartwatch firmware.
 * @date 2026-07-14
 *
 */

#include "lvgl.h"  // IWYU pragma: keep
#include "styles.h"

/**
 * @brief LVGL object representing the notification tile.
 *
 * This object serves as the container for all notification-related UI elements,
 * including BLE status, notification bubbles, and an empty state label.
 */
extern lv_obj_t* ui_tile_notification;

/** @brief LVGL label for displaying BLE status.
 */
extern lv_obj_t* ble_status_label;

/** @brief LVGL label for displaying "no notifications" placeholder.*/
extern lv_obj_t* notification_empty_label;

/** @brief LVGL object for the scrollable container holding notification
 * bubbles.
 *
 * This object is a scrollable container that holds individual notification
 * bubbles. It allows users to scroll through multiple notifications if they
 * exceed the visible area of the notification tile.
 */
extern lv_obj_t* notification_scroll_content;

/** @brief Counter for the number of active notifications.
 *
 * This variable keeps track of the number of notification bubbles currently
 * displayed in the notification tile. It is used to manage the addition and
 * removal of notifications, ensuring that the maximum limit is not exceeded.
 */
extern int notification_count;

/**
 * @brief Creates the notification tile UI component.
 *
 * This function initializes and configures the notification tile, which
 * includes a BLE status label, a scrollable container for notification bubbles,
 * and an empty state label. It applies appropriate styles to each element and
 * positions them within the provided notification_tile object.
 *
 * @param notification_tile The parent LVGL object where the notification tile
 * will be created.
 */
void create_notification_tile(lv_obj_t* notification_tile);

/**
 * @brief
 *
 * @param app
 * @param message
 */
void notification_screen_add_notification(const char* app, const char* message);

/**
 * @brief
 *
 * @param connected
 */
void notification_screen_set_ble_status(bool connected);