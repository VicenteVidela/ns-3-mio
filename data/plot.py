import sys
import matplotlib.pyplot as plt
import numpy as np

# Check if the correct number of command-line arguments is provided
# if len(sys.argv) != 2:
#   print("Usage: plot.py <input_file>")
#   sys.exit(1)

# Initialize lists to store values
seconds = []
throughput = []
packet_delivery_ratio = []
packet_loss = []
mean_delay = []
mean_jitter = []

for i in range(10):
  file_name = 'data/5Desconexiones'+str(i)+'.dat'
  with open(file_name, 'r') as f:
    data = f.read()

  # Split the data into individual entries
  entries = data.strip().split('\n\n')

  noMuerto = True
  # Parse each entry and extract values
  for entry in entries:
    lines = entry.split('\n')
    second = int(lines[0].split(": ")[1])
    second_index = (second-2)//60
    if i==0:
      seconds.append(second)
      throughput.append([])
      packet_delivery_ratio.append([])
      packet_loss.append([])
      mean_delay.append([])
      mean_jitter.append([])
    t = float(lines[1].split(": ")[1].split()[0])
    if t > 0:
      throughput[second_index].append(t)
      packet_delivery_ratio[second_index].append(float(lines[2].split(": ")[1][:-1]))
      packet_loss[second_index].append(float(lines[3].split(": ")[1][:-1]))
      mean_delay[second_index].append(float(lines[4].split(": ")[1].split()[0]))
      mean_jitter[second_index].append(float(lines[5].split(": ")[1].split()[0]))
    else:
      if noMuerto:
        throughput[second_index].append(0)
        packet_delivery_ratio[second_index].append(float(lines[2].split(": ")[1][:-1]))
        packet_loss[second_index].append(0)
        mean_delay[second_index].append(0)
        mean_jitter[second_index].append(0)
        noMuerto = False
      else:
        throughput[second_index].append(0)
        packet_delivery_ratio[second_index].append(None)
        packet_loss[second_index].append(None)
        mean_delay[second_index].append(None)
        mean_jitter[second_index].append(None)

for i in range(len(seconds)):
  throughput[i] = np.mean(throughput[i])
  packet_delivery_ratio[i] = np.mean(packet_delivery_ratio[i])
  packet_loss[i] = np.mean(packet_loss[i])
  mean_delay[i] = np.mean(mean_delay[i])
  mean_jitter[i] = np.mean(mean_jitter[i])

# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))
fig.suptitle('Average Performance Metrics for 10 runs')

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
