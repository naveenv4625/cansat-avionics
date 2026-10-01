# Build Log

Newest entries at the top.

## 2026-10-01: First read from BMP388 chip ID with raw I2C

![Serial output showing chip ID 0x50](images/2026-10-01_bmp388_first_reading.jpg)

- Connected the BMP388 to the Feather over STEMMA QT (I2C).
- Researched the Arduino `Wire` (I2C) library and the BMP388 datasheet's
  register map before using any sensor-specific library, to understand
  what it does before using it.
- Wrote a raw I2C read of the CHIP_ID register (`0x00`) at the sensor's address (`0x77`).
- **Result:** read `0x50`, which matches the datasheet for the BMP388 sensor.

**Next:** Add the Adafruit BMP3XX library, then read temperature, pressure,
and altitude, and check that the values make sense.

---

## 2026-09-29: Toolchain setup + first upload

![Blink test](images/2026-09-29_blink-test.gif)

- Created a PlatformIO firmware project in VS Code (Adafruit Feather ESP32-S3 No PSRAM, Arduino framework)
- **Problem:** Upload failed with "Could not open COM3, port busy."

  **Fix:** Entered the bootloader manually (hold BOOT, tap RESET) for the
  first upload.
- Successful blink test on ESP32-S3 with serial output

**Next:** Connect and code I2C sensors for bench testing

---

## 2026-09-28: Parts arrived

![Components laid out](images/2026-09-28_components-overview.jpg)

- Received the Adafruit order: ESP32-S3 Feather, LoRa FeatherWing, BMP388, LSM6DSOX, microSD breakout, 400 mAh LiPo, buzzer.
- Found that I need a second Feather + radio for the ground station; ordered along with stacking headers, breadboard, and hookup wire.

**Next:** set up PlatformIO, blink the LED, read both sensors over STEMMA QT.

---

<!-- Template for each entry:

## YYYY-MM-DD: Short title

![What the photo shows](images/YYYY-MM-DD_short-description.jpg)

- What I did
- What went wrong and how I fixed it
- Numbers/results, if any

**Next:** what comes next

-->
