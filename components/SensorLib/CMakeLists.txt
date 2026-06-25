# Trimmed 2026-06-24: this project only uses SensorPCF85063 (RTC) and
# SensorQMI8658 (IMU, planned pedometer feature) from upstream SensorLib.
# Removed: src/touch (board uses esp_lcd_touch_ft5x06 instead),
# src/bosch/* (BHI260/BHI360/BMM150/BMA4xx sensor-hub chips, not on this
# board — also the source of ~45MB of unused firmware blob headers),
# src/pmic (SY6970/BQ25896 chargers, not used — PMU is AXP2101 via
# XPowersLib), src/haptic_drivers, src/platform/arduino (Arduino-only,
# this is a pure ESP-IDF project), and assorted unused sensor headers
# (other RTC chips, magnetometers, light sensors, IO expanders).
set(src_dirs
    src
    src/platform
)
set(include_dirs
    src
    src/platform
    src/sensor
)

idf_component_register(
    SRC_DIRS ${src_dirs} 
    INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_timer 
    esp_driver_gpio 
    esp_driver_i2c  
    esp_driver_spi 
    driver
)

target_compile_options(__idf_SensorLib PRIVATE
    -Wno-error=format
    -Wno-format
)
