import sys
import matplotlib.pyplot as plt

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 2:
  print("Usage: python script_name.py <input_file>")
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
packet_loss = []
mean_latency = []
mean_jitter = []

# Parse each entry and extract values
for entry in entries:
    lines = entry.split('\n')
    seconds.append(int(lines[0].split(": ")[1]))
    throughput.append(float(lines[1].split(": ")[1].split()[0]))
    packet_loss.append(float(lines[2].split(": ")[1][:-1]))
    mean_latency.append(float(lines[3].split(": ")[1].split()[0]))
    mean_jitter.append(float(lines[4].split(": ")[1].split()[0]))

# Plotting
fig, axs = plt.subplots(4, 1, figsize=(10, 12))

# Plot Throughput
axs[0].plot(seconds, throughput, marker='o', linestyle='-', color='b')
axs[0].set_title('Throughput (bytes/second)')
axs[0].set_xlabel('Seconds')
axs[0].set_ylabel('Throughput')

# Plot Packet Loss Percentage
axs[1].plot(seconds, packet_loss, marker='o', linestyle='-', color='g')
axs[1].set_title('Packet Loss Percentage (%)')
axs[1].set_xlabel('Seconds')
axs[1].set_ylabel('Packet Loss Percentage')

# Plot Mean Latency
axs[2].plot(seconds, mean_latency, marker='o', linestyle='-', color='r')
axs[2].set_title('Mean Latency (seconds)')
axs[2].set_xlabel('Seconds')
axs[2].set_ylabel('Mean Latency')

# Plot Mean Jitter
axs[3].plot(seconds, mean_jitter, marker='o', linestyle='-', color='purple')
axs[3].set_title('Mean Jitter (seconds)')
axs[3].set_xlabel('Seconds')
axs[3].set_ylabel('Mean Jitter')

# Adjust layout
plt.tight_layout()

# Show the plot
plt.show()
