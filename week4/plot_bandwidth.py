import numpy as np
import matplotlib.pyplot as plt

# loads the CSV 
data = np.genfromtxt('week4/output/bandwidth_results.csv', delimiter=',', skip_header=1)
sizes = data[:, 0]
avg_times = data[:, 3]

# linear fit, time = m * size + c
m, c = np.polyfit(sizes, avg_times, 1)

# c = latency (intercept, time at zero payload)
# m = 1/bandwidth (slope, seconds per byte)
# bandwidth = 1/m (bytes per second)
latency = c
bandwidth = 1.0 / m


plt.figure(figsize=(10, 6))

plt.scatter(sizes, avg_times,s=60, label='Measured')
plt.plot(sizes, m * sizes + c, 'r-', label=f'Fit: t = {m:.2e}*s + {c:.2e}')

plt.xlabel('Message size (bytes)', fontsize=22)
plt.ylabel('Average time (s)', fontsize=22)

plt.tick_params(axis='both', labelsize=20)

plt.legend(fontsize=15)
plt.text(0.05, 0.95,
         f'Latency (c): {latency*1e6:.2f} us\nBandwidth (1/m): {bandwidth/1e6:.2f} MB/s',
         transform=plt.gca().transAxes, verticalalignment='top',
         fontsize=15, bbox=dict(boxstyle='round', facecolor='wheat'))

plt.tight_layout()
plt.savefig('week4/output/bandwidth_plot.svg')
plt.show()

print(f"Latency (c): {latency*1e6} us")
print(f"Bandwidth (1/m): {bandwidth/1e6} MB/s")