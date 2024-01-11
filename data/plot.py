import sys
import matplotlib.pyplot as plt

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
seconds = []
throughput = []
packet_delivery_ratio = []
packet_loss = []
mean_delay = []
mean_jitter = []

# Parse each entry and extract values
for entry in entries:
  lines = entry.split('\n')
  try:
    seconds.append(float(lines[0].split(": ")[1]))
    throughput.append(float(lines[1].split(": ")[1].split()[0]))
    packet_delivery_ratio.append(float(lines[2].split(": ")[1][:-1]))
    packet_loss.append(float(lines[3].split(": ")[1][:-1]))
    mean_delay.append(float(lines[4].split(": ")[1].split()[0]))
    mean_jitter.append(float(lines[5].split(": ")[1].split()[0]))
  except IndexError:
    pass

# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))

# Plot Throughput
axs[0, 0].plot(seconds, throughput, marker='o', linestyle='-', color='b')
axs[0, 0].set_title('Throughput')
axs[0, 0].set_xlabel('Seconds')
axs[0, 0].set_ylabel('Throughput (bps)')

# Plot Delivery Ratio
axs[0, 1].plot(seconds, packet_delivery_ratio, marker='o', linestyle='-', color='r')
axs[0, 1].set_title('Packet Delivery Ratio')
axs[0, 1].set_xlabel('Seconds')
axs[0, 1].set_ylabel('PDR (%)')

# Plot Packet Loss Percentage
axs[1, 0].plot(seconds, packet_loss, marker='o', linestyle='-', color='g')
axs[1, 0].set_title('Packet Loss Percentage')
axs[1, 0].set_xlabel('Seconds')
axs[1, 0].set_ylabel('Packet Loss (%)')

# Combine Mean Delay and Mean Jitter
axs[1, 1].plot(seconds, mean_delay, marker='o', linestyle='-', color='y', label='Mean Delay')
axs[1, 1].plot(seconds, mean_jitter, marker='o', linestyle='-', color='m', label='Mean Jitter')
axs[1, 1].set_title('Mean Delay and Mean Jitter')
axs[1, 1].set_xlabel('Seconds')
axs[1, 1].set_ylabel('Time (ms)')
axs[1, 1].legend()

# Adjust layout
plt.tight_layout()

# Show the plot
plt.show()
