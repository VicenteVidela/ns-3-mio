import sys
import matplotlib.pyplot as plt
import numpy as np

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 2:
  print("Usage: plot.py <input_file>")
  sys.exit(1)

# Get the input file name from the command-line argument
file_name = sys.argv[1]

with open(file_name, 'r') as f:
  data = f.read()

# Split the data into individual entries
entries = data.strip().split('\n\n')

# Max available bandwidth: 478800 Kbps
# Average throughput: 445165 Kbps
# Throughput to bandwidth ratio: 0.929751
# Packet loss percentage: 0.00475821%
# Average delay: 1601.64 ms
# Average jitter: 0.00951679 ms
# Average queue delay: 126.73 ms

# Initialize lists to store values
nodes_disconnected = []
max_bandwidth = []
throughput = []
throughput_ratio = []
packet_loss = []
mean_delay = []
mean_jitter = []
mean_queue_delay = []

noMuerto = True
# Parse each entry and extract values
for entry in entries:
  lines = entry.split('\n')
  lines.pop(0)
  nodes_disconnected.append(float(lines[0].split(": ")[1]))
  t = float(lines[2].split(": ")[1].split()[0])
  if t > 0:
    throughput.append(t)
    throughput_ratio.append(float(lines[3].split(": ")[1])*100)
    packet_loss.append(float(lines[4].split(": ")[1][:-1]))
    mean_delay.append(float(lines[5].split(": ")[1].split()[0]))
    mean_jitter.append(float(lines[6].split(": ")[1].split()[0]))
  else:
    throughput.append(None)
    throughput_ratio.append(None)
    packet_loss.append(None)
    mean_delay.append(None)
    mean_jitter.append(None)


# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))

# Plot Throughput Ratio
axs[0, 0].plot(nodes_disconnected, throughput_ratio, marker='o', linestyle='-', color='b')
axs[0, 0].set_title('Throughput to Bandwidth Ratio')
axs[0, 0].set_xlabel('Nodes Disconnected')
axs[0, 0].set_ylabel('Ratio (%)')
axs[0, 0].set_xticks(nodes_disconnected)

# Plot Packet Loss Percentage
axs[0, 1].plot(nodes_disconnected, packet_loss, marker='o', linestyle='-', color='g')
axs[0, 1].set_title('Packet Loss Percentage')
axs[0, 1].set_xlabel('Nodes Disconnected')
axs[0, 1].set_ylabel('Packet Loss (%)')
axs[0, 1].set_xticks(nodes_disconnected)

# Plot Mean Delay
axs[1, 0].plot(nodes_disconnected, mean_delay, marker='o', linestyle='-', color='y')
axs[1, 0].set_title('Mean Delay')
axs[1, 0].set_xlabel('Nodes Disconnected')
axs[1, 0].set_ylabel('Time (ms)')
axs[1, 0].set_xticks(nodes_disconnected)

# Plot Mean Jitter
axs[1, 1].plot(nodes_disconnected, mean_jitter, marker='o', linestyle='-', color='m')
axs[1, 1].set_title('Mean Jitter')
axs[1, 1].set_xlabel('Nodes Disconnected')
axs[1, 1].set_ylabel('Time (ms)')
axs[1, 1].set_xticks(nodes_disconnected)

plt.tight_layout()
plt.show()
