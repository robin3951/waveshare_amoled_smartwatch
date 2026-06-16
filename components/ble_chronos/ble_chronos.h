#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize BLE and start advertising as "Chronos" device.
// Must be called after bsp_display_start() (lvgl_port must be running).
void ble_chronos_init(void);

bool ble_chronos_is_connected(void);

#ifdef __cplusplus
}
#endif
