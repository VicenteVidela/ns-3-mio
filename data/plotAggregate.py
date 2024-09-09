import matplotlib.pyplot as plt
import numpy as np

# Path to the file with aggregated metrics
input_file = 'averaged_metrics.dat'

# Initialize lists to store values
nodes_disconnected = []
max_bandwidth = []
throughput = []
throughput_ratio = []
packet_loss = []
mean_delay = []
mean_jitter = []

max_nodes = 300

# Read and parse the aggregated data
with open(input_file, 'r') as f:
    # Skip the header line
    header = f.readline()
    for line in f:
        parts = line.strip().split(',')
        nodes_disconnected.append(float(parts[0]) / max_nodes)
        max_bandwidth.append(float(parts[1]))
        throughput.append(float(parts[2]))
        throughput_ratio.append(float(parts[3]))
        packet_loss.append(float(parts[4]))
        mean_delay.append(float(parts[5]))
        mean_jitter.append(float(parts[6]))

# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))

x_ticks = np.arange(0, 1.1, 0.2)

# Plot Throughput Ratio
axs[0, 0].plot(nodes_disconnected, throughput_ratio, marker='o', linestyle='-', color='b')
axs[0, 0].set_title('Throughput to Bandwidth Ratio')
axs[0, 0].set_xlabel('(1 - p)')
axs[0, 0].set_ylabel('Ratio (%)')
axs[0, 0].set_xticks(x_ticks)

# Plot Packet Loss Percentage
axs[0, 1].plot(nodes_disconnected, packet_loss, marker='o', linestyle='-', color='g')
axs[0, 1].set_title('Packet Loss Percentage')
axs[0, 1].set_xlabel('(1 - p)')
axs[0, 1].set_ylabel('Packet Loss (%)')
axs[0, 1].set_xticks(x_ticks)

# Plot Max Bandwidth
axs[1, 0].plot(nodes_disconnected, max_bandwidth, marker='o', linestyle='-', color='r')
axs[1, 0].set_title('Max Bandwidth')
axs[1, 0].set_xlabel('(1 - p)')
axs[1, 0].set_ylabel('Bandwidth (Kbps)')
axs[1, 0].set_xticks(x_ticks)

# Combined Plot for Mean Delay and Mean Jitter
axs[1, 1].plot(nodes_disconnected, mean_delay, marker='o', linestyle='-', color='y', label='Mean Delay')
axs[1, 1].plot(nodes_disconnected, mean_jitter, marker='o', linestyle='-', color='m', label='Mean Jitter')
axs[1, 1].set_title('Mean Delay and Mean Jitter')
axs[1, 1].set_xlabel('(1 - p)')
axs[1, 1].set_ylabel('Time (ms)')
axs[1, 1].set_xticks(x_ticks)
axs[1, 1].legend()

plt.tight_layout()
plt.show()
