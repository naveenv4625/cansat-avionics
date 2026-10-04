"""
Plot Test 01: barometric altitude during an elevator ride (BMP388).

Run from the repo's top folder:
    python analysis/plot_elevator_test.py

Reads:
    data/2026-10-02_elevator-test_run1_full.txt   (Run 1, complete)
    data/2026-10-02_elevator-test_excerpt.txt      (Run 2 is only in here)
Saves:
    docs/images/2026-10-02_elevator-altitude-plot.png

Altitude is RECOMPUTED from the logged pressure for both runs, using one
sea-level reference. This applies the Run 2 correction described in the
test report (Run 2 was logged with P0 = 1013.25 hPa by mistake).
"""

import re
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# Settings (same values as the test report)
# ---------------------------------------------------------------------------
RUN1_FILE = "data/2026-10-02_elevator-test_run1_full.txt"
EXCERPT_FILE = "data/2026-10-02_elevator-test_excerpt.txt"
OUTPUT_FILE = "docs/images/2026-10-02_elevator-altitude-plot.png"

SEA_LEVEL_HPA = 1011.85   # local weather report, 2026-10-02
GROUND_ELEV_M = 273.0     # topo map elevation
SECONDS_PER_READING = 2   # approx. (delay(2000) in the firmware)

# Colors (light theme)
LINE_COLOR = "#2a78d6"    # blue
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
TEXT_MUTED = "#898781"
GRID_COLOR = "#e1e0d9"


# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------
def read_pressures(text):
    """Return every 'Pressure = ... hPa' value in the text as a list of floats."""
    return [float(p) for p in re.findall(r"Pressure = ([\d.]+)", text)]


def altitude_from_pressure(pressure_hpa):
    """Same formula as Adafruit's readAltitude(), minus ground elevation."""
    return 44330.0 * (1.0 - (pressure_hpa / SEA_LEVEL_HPA) ** 0.1903) - GROUND_ELEV_M


def average(values):
    return sum(values) / len(values)


# ---------------------------------------------------------------------------
# Load data
# ---------------------------------------------------------------------------
with open(RUN1_FILE) as f:
    run1_pressure = read_pressures(f.read())

with open(EXCERPT_FILE) as f:
    excerpt = f.read()

# The excerpt has both runs; Run 2 starts after the "Going back up" label
run2_text = excerpt.split("Going back up", 1)[1]
run2_pressure = read_pressures(run2_text)

run1_alt = [altitude_from_pressure(p) for p in run1_pressure]
run2_alt = [altitude_from_pressure(p) for p in run2_pressure]

# ---------------------------------------------------------------------------
# Height change (same definitions as the test report)
# ---------------------------------------------------------------------------
# Run 1: top = avg of first 5 readings; bottom = avg of first 10 settled
# readings (index 30-39, after the sensor stopped changing)
run1_top = average(run1_alt[:5])
run1_bottom = average(run1_alt[30:40])
run1_change = run1_top - run1_bottom

# Run 2: bottom = avg of first 3 readings; top = last reading in the excerpt
run2_bottom = average(run2_alt[:3])
run2_top = run2_alt[-1]
run2_change = run2_top - run2_bottom

print(f"Run 1 (down): {len(run1_alt)} readings, height change = {run1_change:.1f} m")
print(f"Run 2 (up):   {len(run2_alt)} readings, height change = {run2_change:.1f} m")

# ---------------------------------------------------------------------------
# Plot: two side-by-side panels sharing the same altitude scale
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 2, figsize=(11, 4.8), sharey=True)

runs = [
    (axes[0], run1_alt, "Run 1: 14th floor → 1st floor", run1_top, run1_bottom, run1_change),
    (axes[1], run2_alt, "Run 2: 1st floor → 14th floor", run2_top, run2_bottom, run2_change),
]

for ax, alt, title, top, bottom, change in runs:
    t = [i * SECONDS_PER_READING for i in range(len(alt))]

    # Altitude line with small dots for each reading
    ax.plot(t, alt, color=LINE_COLOR, linewidth=1.8,
            marker="o", markersize=4.5,
            markeredgecolor="white", markeredgewidth=1.0)

    # Arrow showing the measured height change, placed to the right of the data
    x_arrow = t[-1] + 6
    ax.annotate("", xy=(x_arrow, top), xytext=(x_arrow, bottom),
                arrowprops=dict(arrowstyle="<->", color=TEXT_SECONDARY, linewidth=1.2))
    ax.text(x_arrow + 2, (top + bottom) / 2, f"{change:.1f} m",
            va="center", ha="left", fontsize=11, fontweight="bold", color=TEXT_PRIMARY)
    ax.set_xlim(-2, x_arrow + 22)

    # Styling: light hairline grid, no top/right border
    ax.set_title(title, loc="left", fontsize=12, color=TEXT_PRIMARY, pad=10)
    ax.set_xlabel("Time since start of log (s, approx.)", color=TEXT_SECONDARY)
    ax.grid(True, color=GRID_COLOR, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ["top", "right"]:
        ax.spines[side].set_visible(False)
    for side in ["left", "bottom"]:
        ax.spines[side].set_color(GRID_COLOR)
    ax.tick_params(colors=TEXT_MUTED, labelcolor=TEXT_SECONDARY)

axes[0].set_ylabel("Altitude (m, relative)", color=TEXT_SECONDARY)

fig.suptitle("Test 01: BMP388 altitude during elevator ride (2026-10-02)",
             x=0.01, ha="left", fontsize=14, fontweight="bold", color=TEXT_PRIMARY)
fig.text(0.01, 0.915,
         f"Altitude recomputed from logged pressure, P0 = {SEA_LEVEL_HPA} hPa, "
         f"minus {GROUND_ELEV_M:.0f} m ground elevation. Run 2 data is an excerpt "
         f"(readings after arrival trimmed).",
         ha="left", fontsize=9, color=TEXT_MUTED)

fig.tight_layout(rect=[0, 0, 1, 0.93])
fig.savefig(OUTPUT_FILE, dpi=150, facecolor="white")
print(f"Saved plot to {OUTPUT_FILE}")

plt.show()