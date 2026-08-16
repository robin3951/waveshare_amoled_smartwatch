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

#define MAX_NOTIFICATIONS 20

#define BLE_STATUS_LABEL_POS_Y 6
#define BLE_STATUS_LABEL_WIDTH 410
#define BLE_STATUS_LABEL_DISCONNECTED_TEXT "BLE: Disconnected"
#define BLE_STATUS_LABEL_CONNECTED_TEXT "BLE: Connected"

#define NOTIFICATION_SCROLL_CONTENT_POS_Y 30
#define NOTIFICATION_SCROLL_CONTENT_WIDTH 410
#define NOTIFICATION_SCROLL_CONTENT_HEIGHT 456

#define NOTIFICATION_EMPTY_LABEL_TEXT "No new\nNotifications"
#define NOTIFICATION_EMPTY_LABEL_WIDTH 410
#define NOTIFICATION_EMPTY_LABEL_HEIGHT 456

#define NOTIFICATION_BUBBLE_WIDTH 380

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
 * @brief Adds a new notification bubble to the notification tile.
 *
 * @param app The name of the application or sender of the notification.
 * @param message The message content of the notification.
 */
void notification_screen_add_notification(const char* app, const char* message);

/**
 * @brief Sets the BLE connection status on the notification tile.
 *
 * @param connected A boolean indicating whether BLE is connected (true) or
 * disconnected (false).
 */
void notification_screen_set_ble_status(bool connected);