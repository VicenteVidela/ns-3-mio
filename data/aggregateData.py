import os, sys, glob
import math

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 4:
	print("Usage: aggregateData.py <topologia> <forma> <imax>")
	sys.exit(1)

topologia = sys.argv[1]
forma = sys.argv[2]
imax = sys.argv[3]

data_directory = f"../resultados/Topologia1/ataques/{topologia}/{forma}/imax{imax}/"

# Initialize dictionaries to store aggregated sums, squared sums, and counts
aggregated_data = {}

# Process each file in the directory
for file_name in glob.glob(os.path.join(data_directory, '*.dat')):
	with open(file_name, 'r') as f:
		data = f.read()

	# Split the data into individual entries
	entries = data.strip().split('\n\n')

	for entry in entries:
		lines = entry.split('\n')
		num_disconnected = int(lines[1].split(": ")[1])

		# Extract values
		max_bandwidth = float(lines[2].split(": ")[1].split()[0])
		throughput = float(lines[3].split(": ")[1].split()[0])
		throughput_ratio = float(lines[4].split(": ")[1])
		packet_loss = float(lines[5].split(": ")[1][:-1])
		mean_delay = float(lines[6].split(": ")[1].split()[0])
		mean_jitter = float(lines[7].split(": ")[1].split()[0])

		if num_disconnected not in aggregated_data:
			aggregated_data[num_disconnected] = {
				'max_bandwidth': [],
				'throughput': [],
				'throughput_ratio': [],
				'packet_loss': [],
				'mean_delay': [],
				'mean_jitter': []
			}

		aggregated_data[num_disconnected]['max_bandwidth'].append(max_bandwidth)
		aggregated_data[num_disconnected]['throughput'].append(throughput)
		aggregated_data[num_disconnected]['throughput_ratio'].append(throughput_ratio)
		aggregated_data[num_disconnected]['packet_loss'].append(packet_loss)
		aggregated_data[num_disconnected]['mean_delay'].append(mean_delay)
		aggregated_data[num_disconnected]['mean_jitter'].append(mean_jitter)

# Calculate averages, standard deviations, and store in a new file
output_file = f'AggregatedData/{topologia}_{forma}_imax{imax}.dat'

def calculate_std(values, mean):
	if len(values) < 2:
		return 0.0
	variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
	return math.sqrt(variance)

with open(output_file, 'w') as f:
	f.write("Nodes Disconnected,Max Available Bandwidth (Kbps),Std Max Bandwidth,Average Throughput (Kbps),Std Throughput,Throughput to Bandwidth Ratio (%),Std Throughput Ratio,Packet Loss Percentage (%),Std Packet Loss,Average Delay (ms),Std Delay,Average Jitter (ms),Std Jitter\n")
	for num_disconnected in sorted(aggregated_data.keys()):
		values = aggregated_data[num_disconnected]
		count = len(values['max_bandwidth'])

		avg_max_bandwidth = sum(values['max_bandwidth']) / count
		avg_throughput = sum(values['throughput']) / count
		avg_throughput_ratio = sum(values['throughput_ratio']) / count * 100
		avg_packet_loss = sum(values['packet_loss']) / count
		avg_mean_delay = sum(values['mean_delay']) / count
		avg_mean_jitter = sum(values['mean_jitter']) / count

		std_max_bandwidth = calculate_std(values['max_bandwidth'], avg_max_bandwidth)
		std_throughput = calculate_std(values['throughput'], avg_throughput)
		std_throughput_ratio = calculate_std(values['throughput_ratio'], avg_throughput_ratio / 100) * 100
		std_packet_loss = calculate_std(values['packet_loss'], avg_packet_loss)
		std_mean_delay = calculate_std(values['mean_delay'], avg_mean_delay)
		std_mean_jitter = calculate_std(values['mean_jitter'], avg_mean_jitter)

		f.write(f"{num_disconnected},{avg_max_bandwidth},{std_max_bandwidth},{avg_throughput},{std_throughput},{avg_throughput_ratio},{std_throughput_ratio},{avg_packet_loss},{std_packet_loss},{avg_mean_delay},{std_mean_delay},{avg_mean_jitter},{std_mean_jitter}\n")
