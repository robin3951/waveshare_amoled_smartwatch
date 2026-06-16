idf_component_register(
    SRCS "waveshare_amoled_smartwatch.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        waveshare__esp32_s3_touch_amoled_2_06
        esp_lvgl_port
        lvgl
        SensorLib
        XPowersLib
        ui
        ble_chronos
)
