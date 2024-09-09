import os, sys, glob

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 2:
  print("Usage: plot.py <data_directory>")
  sys.exit(1)

# Get the input file name from the command-line argument
data_directory = sys.argv[1]


# Initialize dictionaries to store aggregated sums and counts
aggregated_data = {}
node_count = 0

# Process each file in the directory
for file_name in glob.glob(os.path.join(data_directory, '*.dat')):
  with open(file_name, 'r') as f:
    data = f.read()

  # Split the data into individual entries
  entries = data.strip().split('\n\n')

  for entry in entries:
    lines = entry.split('\n')
    num_disconnected = int(lines[1].split(": ")[1])

    # Extract and accumulate values
    max_bandwidth = float(lines[2].split(": ")[1].split()[0])
    throughput = float(lines[3].split(": ")[1].split()[0])
    throughput_ratio = float(lines[4].split(": ")[1])
    packet_loss = float(lines[5].split(": ")[1][:-1])
    mean_delay = float(lines[6].split(": ")[1].split()[0])
    mean_jitter = float(lines[7].split(": ")[1].split()[0])

    if num_disconnected not in aggregated_data:
      aggregated_data[num_disconnected] = {
        'max_bandwidth': 0.0,
        'throughput': 0.0,
        'throughput_ratio': 0.0,
        'packet_loss': 0.0,
        'mean_delay': 0.0,
        'mean_jitter': 0.0,
        'count': 0
      }

    aggregated_data[num_disconnected]['max_bandwidth'] += max_bandwidth
    aggregated_data[num_disconnected]['throughput'] += throughput
    aggregated_data[num_disconnected]['throughput_ratio'] += throughput_ratio
    aggregated_data[num_disconnected]['packet_loss'] += packet_loss
    aggregated_data[num_disconnected]['mean_delay'] += mean_delay
    aggregated_data[num_disconnected]['mean_jitter'] += mean_jitter
    aggregated_data[num_disconnected]['count'] += 1

# Calculate averages and store in a new file
output_file = 'averaged_metrics.dat'

with open(output_file, 'w') as f:
  f.write("Nodes Disconnected,Max Available Bandwidth (Kbps),Average Throughput (Kbps),Throughput to Bandwidth Ratio (%),Packet Loss Percentage (%),Average Delay (ms),Average Jitter (ms)\n")
  for num_disconnected in sorted(aggregated_data.keys()):
    count = aggregated_data[num_disconnected]['count']
    avg_max_bandwidth = aggregated_data[num_disconnected]['max_bandwidth'] / count
    avg_throughput = aggregated_data[num_disconnected]['throughput'] / count
    avg_throughput_ratio = aggregated_data[num_disconnected]['throughput_ratio'] / count * 100
    avg_packet_loss = aggregated_data[num_disconnected]['packet_loss'] / count
    avg_mean_delay = aggregated_data[num_disconnected]['mean_delay'] / count
    avg_mean_jitter = aggregated_data[num_disconnected]['mean_jitter'] / count

    f.write(f"{num_disconnected},{avg_max_bandwidth},{avg_throughput},{avg_throughput_ratio},{avg_packet_loss},{avg_mean_delay},{avg_mean_jitter}\n")

