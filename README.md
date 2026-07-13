# ESP32-S3 AMOLED Smartwatch

Custom firmware and Android companion app for the
[Waveshare ESP32-S3-Touch-AMOLED-2.06](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.06) —
a 2.06" AMOLED smartwatch development board based on the ESP32-S3.
Communicates with an Android companion app via the Chronos BLE protocol.

[![Firmware CI](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/firmware-ci.yml/badge.svg)](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/firmware-ci.yml)
[![Android CI](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/android-ci.yml/badge.svg)](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/android-ci.yml)
[![Release](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/release.yml/badge.svg)](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/release.yml)
[![Secret Scan](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/secret-scan.yml/badge.svg)](https://github.com/robin3951/waveshare_amoled_smartwatch/actions/workflows/secret-scan.yml)

<!-- TODO: add photo or demo GIF of the watch here -->

---

## Hardware

| Component | Chip | Role |
|-----------|------|------|
| MCU | ESP32-S3 | Dual-core 240 MHz, 16 MB Flash, 8 MB PSRAM |
| Display | SH8601 | 2.06" AMOLED, 410 × 502 px |
| Touch | FT3168 | Capacitive touchscreen, I2C |
| PMU | AXP2101 | LiPo charging, fuel gauge (E-Gauge 3.0) |
| IMU | QMI8658 | 6-axis accelerometer + gyroscope |
| RTC | PCF85063 | Real-time clock with hardware alarm |
| Audio | ES8311 + ES7210 | Speaker codec + microphone array |
| Storage | TF card slot | Not yet used in firmware |

→ Full pinout, schematics and datasheets on the
[Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.06).

### Hardware Notes

- **Step counter:** The QMI8658 hardware pedometer is non-functional on this
  board revision (REV_ID `0x7C`). The firmware uses a software algorithm
  (IIR filter + peak detection on the accelerometer) instead — no action
  needed from the user.
- **Backlight control:** `backlight_off` sets the brightness register to 0
  but does not send a DISPOFF or SLPIN command to the panel. The display
  wakes significantly faster this way, which is intentional.
- **USB port:** Use the data-capable USB-C port for flashing. Both ports
  look identical on the board — the correct one is labeled on the PCB silkscreen.

---

## Features

- Clock face with RTC time sync via companion app
- BLE notifications and call alerts (Chronos protocol)
- Software step counter (IIR filter + peak detection on QMI8658)
- Battery indicator (AXP2101 fuel gauge — percentage + voltage)
- Auto screen-off after 15 s inactivity, touch-to-wake

→ [What's in progress and planned (Project Board)](https://github.com/users/robin3951/projects/2)

---

## Getting Started

### Prerequisites

| Tool | Version |
|------|---------|
| [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/) | v5.x |
| Python | ≥ 3.8 |
| CMake | ≥ 3.16 (bundled with ESP-IDF) |

### Build & Flash

```bash
git clone https://github.com/robin3951/waveshare_amoled_smartwatch
cd waveshare_amoled_smartwatch
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor   # Linux / macOS
idf.py -p COM3 flash monitor            # Windows (adjust port in Device Manager)
```

### First Boot

The watch displays the clock face immediately after flashing. Time is not
set until the companion Android app syncs it over BLE. Open the app, pair
with the watch, and time syncs automatically.

---

## BLE / Companion App

The firmware implements the
[Chronos BLE protocol](https://github.com/fbiego/chronos-esp32),
making it compatible with the companion Android app in this repository.

| Direction | Data |
|-----------|------|
| Android → Watch | Time sync, notifications, call alerts |
| Watch → Android | Step count, battery level |

The companion app source lives in the `android companion app/` directory
of this repo.

---

## Project Structure

- 📁 [`main/`](main/) — firmware entry point and FreeRTOS tasks
  - [`waveshare_amoled_smartwatch.cpp`](main/waveshare_amoled_smartwatch.cpp) — entry point, task startup
  - [`hardware.cpp`](main/hardware.cpp) — peripheral init (display, IMU, PMU, touch)
  - 📁 [`tasks/`](main/tasks/) — all FreeRTOS tasks
    - [`clock_task`](main/tasks/clock_task.cpp) — RTC read → UI (1 s)
    - [`battery_task`](main/tasks/battery_task.cpp) — AXP2101 SoC → UI (5 s)
    - [`pedometer_task`](main/tasks/pedometer_task.cpp) — IMU polling → step counter (20 ms)
    - [`screen_timeout_task`](main/tasks/screen_timeout_task.cpp) — inactivity → backlight off (200 ms)
    - [`step_counter`](main/tasks/step_counter.cpp) — IIR filter + peak/valley state machine
- 📁 [`components/`](components/) — bundled libraries
  - 📁 [`ui/`](components/ui/) — LVGL screens, styles, fonts ← **custom**
  - 📁 [`ble_chronos/`](components/ble_chronos/) — Chronos BLE protocol implementation ← **custom**
  - [`waveshare__esp32_s3_touch_amoled_2_06/`](components/waveshare__esp32_s3_touch_amoled_2_06/) — Board Support Package
  - [`XPowersLib/`](components/XPowersLib/) — AXP2101 PMU driver
  - [`SensorLib/`](components/SensorLib/) — QMI8658 IMU + FT3168 touch drivers
- 📁 [`android-companion/`](android-companion/) — companion Android app (Kotlin)
- 📁 [`docs/`](docs/) — architecture diagrams, Doxygen config
- 📁 [`.github/`](.github/) — CI workflows, issue templates

---

## Documentation

Auto-generated and deployed on every push to `main`:

- [Firmware API (Doxygen)](https://robin3951.github.io/waveshare_amoled_smartwatch/firmware/index.html)
- [Android Companion App (Dokka)](https://robin3951.github.io/waveshare_amoled_smartwatch/android/index.html)

---

## Releases

Pre-built firmware binaries are published automatically for each milestone
on the [Releases page](https://github.com/robin3951/waveshare_amoled_smartwatch/releases).

Flash without setting up ESP-IDF:

```bash
pip install esptool
esptool.py -p /dev/ttyUSB0 write_flash 0x0 firmware.bin
```

---

## Acknowledgements

- [lewisxhe/XPowersLib](https://github.com/lewisxhe/XPowersLib) — AXP2101 PMU driver
- [lewisxhe/SensorLib](https://github.com/lewisxhe/SensorLib) — QMI8658 IMU + FT3168 touch driver
- [fbiego/chronos-esp32](https://github.com/fbiego/chronos-esp32) — Chronos BLE protocol reference
- [Waveshare](https://www.waveshare.com) — Board Support Package and hardware documentation

---

## Related Resources

- [Waveshare ESP32-S3-Touch-AMOLED-2.06 Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.06)
- [Official Waveshare Example Repo](https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.06)
- [LVGL Documentation](https://docs.lvgl.io)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/)
