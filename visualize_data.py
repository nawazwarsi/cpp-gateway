import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("sensor_fusion_log.csv")

# Convert Timestamp to datetime
df['Timestamp'] = pd.to_datetime(df['Timestamp'], errors='coerce')

# Pivot table to plot each sensor separately
pivot = df.pivot(index='Timestamp', columns='Sensor', values='Value')

pivot.plot(figsize=(10,5), marker='o')
plt.title("ADAS Sensor Fusion Data Over Time")
plt.xlabel("Timestamp")
plt.ylabel("Sensor Values")
plt.legend()
plt.grid(True)
plt.show()
