idf_component_register(
    SRCS "ble_chronos.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash esp_lvgl_port ui
)
