import glob
import os
import matplotlib.pyplot as plt
import pandas as pd

interval_time = 75  # ms per sample
power_column_name = "Power (mW)"

csv_files = glob.glob("power_log_*.csv")

if not csv_files:
    raise FileNotFoundError("No matching CSV files found.")

plt.figure(figsize=(10, 6))

all_powers = []
common_time = None

for i, file in enumerate(csv_files):
    df = pd.read_csv(file)
    time_ms = df.index * interval_time

    # Save the time array from the first file for the x-axis
    if common_time is None:
        common_time = time_ms

    all_powers.append(df[power_column_name])

    # Plot individual run
    plt.plot(
        time_ms,
        df[power_column_name],
        color="gray",
        alpha=0.3,
        label="Individual Runs" if i == 0 else "",
    )

mean_power = pd.concat(all_powers, axis=1).mean(axis=1)

plt.plot(
    common_time[: len(mean_power)],
    mean_power,
    color="blue",
    linewidth=2,
    label="Mean Power",
)

plt.xlim(0, None)
plt.ylim(0, None)
plt.xlabel("Time (ms)")
plt.ylabel("Power (mW)")
plt.title("Power Consumption Over Time (QVGA)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()