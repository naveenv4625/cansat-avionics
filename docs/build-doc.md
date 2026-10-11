# Build Log

Newest entries at the top.

## 2026-10-10: Accelerometer 6-position calibration (run 1)
 
- Fixed the remaining bugs in the calibration tool (`src/accel_cal.cpp`) and ran the
  6-position calibration: each axis pointed straight up, then straight down, averaging
  500 readings of all three axes per position.
- **Results (run 1, hand-clamped fixture):**
  | Axis | Up (m/s²) | Down (m/s²) | Offset (m/s²) | Scale |
  |---|---|---|---|---|
  | X | 9.85 | −9.88 | −0.0126 | 1.0061 |
  | Y | 9.79 | −9.93 | −0.0677 | 1.0057 |
  | Z | 9.97 | −9.69 | +0.1393 | 1.0026 |
  
  Offsets ≤ 0.14 m/s², scale errors 0.3–0.6%. Worst uncorrected reading was
  0.16 m/s² off (Z up).
- Cross-check: X's sideways readings agreed with its up/down offset; Z's disagreed by
  ~0.17 m/s², consistent with ~0.5–1° of tilt when the board was stood on edge.
- The board was held between two boxes by hand, so I'm redoing the calibration with the
  board fixed to a block and hands off during measurement. Run 1 is kept for comparison
  (repeatability check).
- Raw log: `data/2026-10-10_accel-calibration-run1-handheld.txt`
**Next:** Calibration run 2 with a proper fixture, then a verification run with the
correction applied, then the Test 02 report.
 
---
 
## 2026-10-05: Multi-environment PlatformIO setup + calibration tool
 
- Restructured `platformio.ini` into a shared `[env]` plus two environments:
  `main` (flight code) and `accel_cal` (calibration tool), using `build_src_filter`
  so each builds a different file from `src/`.
- Enabled PlatformIO's `log2file` monitor filter so every Serial Monitor session is
  saved to a log file automatically (estimated ~2 KB/s, ~7 MB per hour of logging).
- Wrote the first version of the accelerometer 6-position calibration tool.
**Next:** Debug the calibration tool and run the calibration.
 
---
 
## 2026-10-04: Startup calibration for barometer and gyro
 
- Added a startup **gyro bias calibration**: average 500 readings while still, then
  subtract the offsets from every reading. The gyro now reads ≈ 0 rad/s at rest.
- Added a startup **barometer ground calibration**: average the pressure at power-up and
  use it as the altitude reference, so altitude reads ≈ 0 m at the starting point.
  Altitude is computed from the same reading's pressure instead of `readAltitude()`
  (which takes a second, separate reading).
- **Problem:** Both calibrations "averaged" a single reading.
  **Cause:** The sensor read was outside the averaging loop, so the same value was
  added hundreds of times.
  **Fix:** Moved the read inside the loop.
- **Problem:** Altitude read ~3 m off right after calibration.
  **Cause:** The first readings after configuring the BMP388 hadn't settled yet and
  pulled the ground-pressure average off.
  **Fix:** Discard the first 50 readings before averaging. Altitude now starts at
  ≈ 0 m, with ~±0.5 m wander at rest (normal noise and drift).
- Calibration results print as `#` comment lines before the CSV header so every log
  records them.
- Decided: gyro bias is recalibrated every startup (it changes each power-on and with
  temperature); accelerometer offset/scale will be calibrated once with the 6-position
  method and hard-coded (gravity must still read 9.81 m/s²).
**Next:** Accelerometer 6-position calibration.
 
---

## 2026-10-03: Test 01 analysis, plotting script, LSM6DSOX bring-up

![Test 01 altitude plot](images/2026-10-02_elevator-altitude-plot.png)

- Wrote the [Test 01 report](tests/01-elevator-altitude-test.md) and committed the
  exact firmware used for the test.
- Wrote `analysis/plot_elevator_test.py`: parses the Serial logs, recomputes altitude
  from raw pressure (applying the Run 2 reference correction), calculates the height
  change for each run, and saves the plot above.
- Connected the LSM6DSOX by daisy-chaining it off the BMP388's second STEMMA QT port
  (shared I2C bus: BMP388 at 0x77, LSM6DSOX at 0x6A).
- Configured ±16 g accelerometer and ±2000 °/s gyro ranges, and now read
  acceleration, rotation rate, and temperature alongside the barometer.
- Switched Serial output to one CSV line per reading with a `millis()` timestamp, and
  enabled PlatformIO's `time` and `log2file` monitor filters so every session is saved
  automatically.
- Result: <e.g. flat on desk: ax ≈ 0, ay ≈ 0, az ≈ 9.8 m/s²; gyro ≈ 0 at rest>

**Next:** IMU sanity checks (flip test), battery monitor, startup ground calibration,
then Test 02 (elevator test with barometer + accelerometer).

---

## 2026-10-02: Elevator altitude test (BMP388)

- Rode an elevator between the 1st and 14th floors while logging pressure and altitude.
- **Result:** Measured 38.3 m going down and 37.6 m going up (~38 m, ~2.9 m per floor);
  The two runs agree within 0.7 m. Elevator speed ≈ 1.6 m/s.
- **Problem:** Run 2 was logged with the wrong sea-level reference pressure.
  **Fix:** Recomputed altitude from the raw pressure readings. Height difference unaffected.
- After stopping, readings settled in ~18 s, with each reading closing ~25% of the
  remaining gap, matching IIR filter coefficient 3 updating once per 2 s reading.
- Pressure drift on the 1st floor was only ~0.3 m over 10 minutes.
- Full report: [Test 01](tests/01-elevator-altitude-test.md)

**Next:** Write up the report, plot the data, connect the LSM6DSOX IMU.

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
