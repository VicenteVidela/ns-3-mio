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

# Initialize lists to store values
nodes_disconnected = []
throughput = []
packet_loss = []
mean_delay = []
mean_jitter = []

noMuerto = True
# Parse each entry and extract values
for entry in entries:
  lines = entry.split('\n')
  nodes_disconnected.append(float(lines[0].split(": ")[1]))
  t = float(lines[1].split(": ")[1].split()[0])
  if t > 0:
    throughput.append(t)
    packet_loss.append(float(lines[2].split(": ")[1][:-1]))
    mean_delay.append(float(lines[3].split(": ")[1].split()[0]))
    mean_jitter.append(float(lines[4].split(": ")[1].split()[0]))
  else:
    throughput.append(None)
    packet_loss.append(None)
    mean_delay.append(None)
    mean_jitter.append(None)


# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))

# Plot Throughput
axs[0, 0].plot(nodes_disconnected, throughput, marker='o', linestyle='-', color='b')
axs[0, 0].set_title('Throughput')
axs[0, 0].set_xlabel('Nodes Disconnected')
axs[0, 0].set_ylabel('Throughput (bps)')
axs[0, 0].set_xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 1.0))

# Plot Packet Loss Percentage
axs[0, 1].plot(nodes_disconnected, packet_loss, marker='o', linestyle='-', color='g')
axs[0, 1].set_title('Packet Loss Percentage')
axs[0, 1].set_xlabel('Nodes Disconnected')
axs[0, 1].set_ylabel('Packet Loss (%)')
axs[0, 1].set_xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 1.0))

# Combine Mean Delay and Mean Jitter
axs[1, 0].plot(nodes_disconnected, mean_delay, marker='o', linestyle='-', color='y', label='Mean Delay')
axs[1, 0].plot(nodes_disconnected, mean_jitter, marker='o', linestyle='-', color='m', label='Mean Jitter')
axs[1, 0].set_title('Mean Delay and Mean Jitter')
axs[1, 0].set_xlabel('Nodes Disconnected')
axs[1, 0].set_ylabel('Time (ms)')
axs[1, 0].set_xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 1.0))
axs[1, 0].legend()

# # Plot Mean Delay
# axs[1, 0].plot(nodes_disconnected, mean_delay, marker='o', linestyle='-', color='y')
# axs[1, 0].set_title('Mean Delay')
# axs[1, 0].set_xlabel('Nodes Disconnected')
# axs[1, 0].set_ylabel('Time (ms)')
# axs[1, 0].set_xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 1.0))

# # Plot Mean Jitter
# axs[1, 1].plot(nodes_disconnected, mean_jitter, marker='o', linestyle='-', color='m')
# axs[1, 1].set_title('Mean Jitter')
# axs[1, 1].set_xlabel('Nodes Disconnected')
# axs[1, 1].set_ylabel('Time (ms)')
# axs[1, 1].set_xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 1.0))

# fig.xticks(np.arange(min(nodes_disconnected), max(nodes_disconnected)+1, 2.0))
# Adjust layout
plt.tight_layout()

# Show the plot
plt.show()
