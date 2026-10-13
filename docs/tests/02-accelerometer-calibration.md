# Test 02: Accelerometer 6-Position Calibration

**Dates:** 2026-10-10 (calibration runs 1 and 2), 2026-10-13 (verification)

**Hardware:** Adafruit ESP32-S3 Feather + LSM6DSOX 6-DoF IMU (STEMMA QT, I2C 0x6A), powered over USB from a laptop

**Firmware:** `firmware/src/accel_cal.cpp`, built with the `accel_cal` environment (calibration) and the `accel_verify` environment (verification) <!-- add "tagged test-02" once you create the tag -->

**Data:**
- [`data/2026-10-10_accel-calibration-run1-hand-clamped.txt`](../../data/2026-10-10_accel-calibration-run1-hand-clamped.txt): Run 1, board clamped between two boxes by hand
- [`data/2026-10-10_accel-calibration-run2-box.txt`](../../data/2026-10-10_accel-calibration-run2-box.txt): Run 2, board held by a box, hands off during measurement
- [`data/2026-10-13_accel-verification.txt`](../../data/2026-10-13_accel-verification.txt): verification run with the correction applied.

---

## Objective

Measure the offset and scale error of each accelerometer axis, derive a correction for the flight firmware, and verify that the corrected readings match gravity in all six orientations.

## Background

A real accelerometer axis doesn't read exactly ±9.81 m/s² when it points straight up or down. Each axis has:
- an **offset** (bias): a constant added to every reading
- a **scale error**: the reading is slightly too large or too small in proportion to the true value

Pointing each axis straight up (+1 g) and straight down (−1 g) gives two equations per axis, which is enough to solve for both:

```
offset = (up + down) / 2
scale  = (up − down) / (2 × 9.80665)
corrected = (raw − offset) / scale
```

Tilt barely affects the up/down readings. A tilt θ only reduces the on-axis reading by a factor of cos θ, so 1° costs just 0.0015 m/s². This is why the method doesn't need a precision fixture.

## Setup

| Setting | Value |
|---|---|
| Accelerometer range | ±16 g |
| Output data rate | 104 Hz |
| Samples per position | 500, one every ~10 ms (~5 s per position) |
| Settling delay before sampling | 1.5 s after pressing Enter |
| Reference gravity | 9.80665 m/s² (standard gravity) |
| Room temperature | <!-- fill in if known, otherwise write "not recorded" --> |

The tool prompts for each of the six orientations (+Z, −Z, +X, −X, +Y, −Y up), waits for Enter, averages all three axes over 500 samples, and prints the averages plus the offset and scale for each axis.

**Verification mode:** the `accel_verify` environment builds the same tool with a `-DVERIFY_MODE` flag. In this mode, each sample is corrected *before* averaging, using the same constants header the flight code uses (`firmware/include/accel_calibration.h`). The tool also prints the magnitude of each position's averaged vector, √(x² + y² + z²), which doesn't depend on tilt.

## Procedure

1. **Run 1 (2026-10-10):** measured all six positions with the board clamped between two boxes by hand.
2. **Run 2 (2026-10-10):** repeated the measurement with the board held by a box and hands off during sampling, as a repeatability check.
3. Averaged the run 1 and run 2 constants and saved them in the shared header used by both the flight code and the verification build.
4. **Pass criteria (set before the verification run):** in every position, the corrected on-axis reading is within **0.05 m/s²** of ±9.80665, and the magnitude is within **0.05 m/s²** of 9.80665. Off-axis readings up to ~0.4 m/s² are acceptable (≈2° of fixture tilt).
5. **Verification (2026-10-13):** measured all six positions again with the correction applied. <!-- describe the fixture: same box setup as run 2? -->

## Results

### Calibration runs (uncorrected)

On-axis averages (printed to 2 decimals in runs 1 and 2):

| Position | Run 1 (m/s²) | Run 2 (m/s²) | Ideal (m/s²) |
|---|---|---|---|
| +Z up | 9.97 | 9.95 | +9.81 |
| −Z up | −9.69 | −9.70 | −9.81 |
| +X up | 9.85 | 9.87 | +9.81 |
| −X up | −9.88 | −9.85 | −9.81 |
| +Y up | 9.79 | 9.77 | +9.81 |
| −Y up | −9.93 | −9.95 | −9.81 |

Calibration constants:

| Axis | Run 1 offset (m/s²) | Run 1 scale | Run 2 offset (m/s²) | Run 2 scale | **Used: average** |
|---|---|---|---|---|---|
| X | −0.0126 | 1.0061 | +0.0090 | 1.0057 | **−0.0018 / 1.0059** |
| Y | −0.0677 | 1.0057 | −0.0919 | 1.0057 | **−0.0798 / 1.0057** |
| Z | +0.1393 | 1.0026 | +0.1221 | 1.0020 | **+0.1307 / 1.0023** |

<!-- Check the Run 2 offset/scale values against the offset/scale lines printed in the
     run 2 log. They were back-calculated from the averaged constants, so the last digit
     may differ. Replace them with the printed values if so. -->

The largest errors were on the Z axis (offset +0.13 m/s²). All scale errors were between +0.2% and +0.6%.

### Verification run (corrected)

Error = |reading| − 9.80665, so a positive error means the reading is too large.

| Position | Corrected on-axis (m/s²) | On-axis error | Magnitude (m/s²) | Magnitude error | Result |
|---|---|---|---|---|---|
| +Z up | 9.8056 | −0.001 | 9.8111 | +0.004 | ✅ Pass |
| −Z up | −9.7980 | −0.009 | 9.8063 | 0.000 | ✅ Pass |
| +X up | 9.8152 | +0.009 | 9.8166 | +0.010 | ✅ Pass |
| −X up | −9.8132 | +0.007 | 9.8135 | +0.007 | ✅ Pass |
| +Y up | 9.8104 | +0.004 | 9.8140 | +0.007 | ✅ Pass |
| −Y up | −9.8058 | −0.001 | 9.8088 | +0.002 | ✅ Pass |

**All six positions passed.** The largest on-axis error was **0.009 m/s²** and the largest magnitude error **0.010 m/s²**, both about 5× inside the 0.05 m/s² limit.

Offset and scale recalculated from the corrected data (ideal: 0 and 1):

| Axis | Leftover offset (m/s²) | Leftover scale |
|---|---|---|
| X | 0.0010 | 1.0008 |
| Y | 0.0023 | 1.0001 |
| Z | 0.0038 | 0.9995 |

### Before vs. after (same verification run)

Uncorrected values were back-calculated from the corrected ones with `raw = corrected × scale + offset`, the exact inverse of the correction. This compares the same measurements with and without calibration.

| Position | Uncorrected (m/s²) | Error | Corrected (m/s²) | Error |
|---|---|---|---|---|
| +Z up | 9.959 | +0.152 | 9.806 | −0.001 |
| −Z up | −9.690 | −0.117 | −9.798 | −0.009 |
| +X up | 9.871 | +0.065 | 9.815 | +0.009 |
| −X up | −9.873 | +0.066 | −9.813 | +0.007 |
| +Y up | 9.787 | −0.020 | 9.810 | +0.004 |
| −Y up | −9.942 | +0.135 | −9.806 | −0.001 |
| **Worst** | | **0.152** | | **0.009** |
| **Mean (absolute)** | | **0.092** | | **0.005** |

**Calibration cut the worst-case on-axis error from 0.152 to 0.009 m/s² (~17×) and the mean error from 0.092 to 0.005 m/s².** The worst magnitude error fell from 0.160 to 0.010 m/s².

### Repeatability across three sessions

The verification run also gives a third, independent estimate of the calibration constants: offset = used offset + used scale × leftover offset, and scale = used scale × leftover scale.

| Axis | Offset: run 1 / run 2 / Oct 13 (m/s²) | Spread | Scale: run 1 / run 2 / Oct 13 | Spread |
|---|---|---|---|---|
| X | −0.0126 / +0.0090 / −0.0008 | 0.022 | 1.0061 / 1.0057 / 1.0067 | 0.10% |
| Y | −0.0677 / −0.0919 / −0.0775 | 0.024 | 1.0057 / 1.0057 / 1.0058 | 0.01% |
| Z | +0.1393 / +0.1221 / +0.1345 | 0.017 | 1.0026 / 1.0020 / 1.0018 | 0.08% |

Over three sessions on two different days, the offsets agreed within **0.025 m/s²** and the scales within **0.1%**.

## Analysis

1. **The calibration works and is stable.** Corrected readings matched gravity within 0.01 m/s² in every orientation, three days after the constants were measured. The leftover error is about the size of the session-to-session variation (scale differences of up to 0.1% are ~0.01 m/s² at 1 g), so more careful calibration runs wouldn't improve it much.

2. **Magnitude separates tilt from calibration error.** In the −Z position, the on-axis reading was 0.009 m/s² low, but the magnitude was within 0.0004 m/s². The sideways readings show the board was tilted ~2.4° there, and a 2.4° tilt reduces the on-axis reading by 9.81 × (1 − cos 2.4°) ≈ 0.008 m/s². So almost all of that shortfall is tilt, not calibration error, which is why magnitude is the better pass/fail check.

3. **Off-axis readings measure fixture tilt, not sensor error.** The sideways readings were 0.02–0.40 m/s², which corresponds to the board sitting 0.4–2.4° off vertical. Sideways readings change with the *sine* of the tilt (1° ≈ 0.17 m/s²), so they're far more sensitive to how the board sits than up/down readings are. Where tilt was small, they agreed with the up/down result: the Z offset estimated from the two Y positions (+0.006 m/s²) matches the leftover Z offset (+0.004 m/s²).

4. **Why this matters for flight.** Velocity and position estimated from acceleration add up any constant error over time. Over a 10 s segment, the uncorrected 0.15 m/s² Z error would add up to 1.5 m/s of velocity error and 7.5 m of position error. With the corrected ~0.01 m/s², that's ~0.1 m/s and ~0.5 m.

5. **Reference gravity.** The calibration used standard gravity (9.80665 m/s²). Local gravity in Ann Arbor is about 9.803 m/s² (1980 International Gravity Formula at 42.3° N, plus the free-air correction for 273 m), which is 0.04% lower. This means the calibrated scale is ~0.04% high in absolute terms, which is smaller than the session-to-session repeatability. It's negligible for this payload, but local g could replace 9.80665 in a future calibration.

## Limitations

- **Per-axis correction only.** Offset and scale are corrected for each axis separately. Cross-axis effects (sensor axes not perfectly perpendicular, or one axis responding slightly to acceleration along another) aren't corrected. That would need a full 3×3 calibration with more orientations.
- **Room temperature only.** Offset and scale can drift with temperature, and temperature wasn't logged. Flight temperatures will be close to ground conditions, but the drift hasn't been measured.
- **Rounded raw data.** Runs 1 and 2 printed the averages to 2 decimals (offset and scale lines had 4), so the uncorrected position table is rounded to 0.01 m/s². The verification run printed 4 decimals.
- **Constants not in the log.** The verification log doesn't record which constants or which mode were used. The before/after table assumes the averaged constants above.

## Lessons / changes for next test

- Print a `#` header line at the start of every log with the mode (calibration or verify), the constants in use, and the date, so each log is self-describing
- Print all averages to 4 decimals (done for the verification run)
- Record room temperature, or log the IMU's temperature sensor alongside each position
- Refactor the six copied measurement blocks into one `measurePosition()` function. A bug where each reading was summed twice had to be fixed in six places before the verification run
- Repeat the verification before flight to confirm the constants are still good
- **Next:** static noise test (accelerometer and barometer standard deviation at rest)
