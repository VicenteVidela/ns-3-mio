import matplotlib.pyplot as plt
import numpy as np
import csv

# File containing the averaged data
input_file = 'averaged_metrics.dat'

# Initialize lists to store data
num_disconnected = []
throughput_ratio = []
std_throughput_ratio = []

# Read the file
with open(input_file, 'r') as f:
	reader = csv.reader(f)
	next(reader)  # Skip header

	for row in reader:
		num_disconnected.append(int(row[0]))
		throughput_ratio.append(float(row[5]))
		std_throughput_ratio.append(float(row[6]))

# Convert lists to numpy arrays
num_disconnected = np.array(num_disconnected)
throughput_ratio = np.array(throughput_ratio)
std_throughput_ratio = np.array(std_throughput_ratio)

# Plot data with error bars
plt.figure(figsize=(8, 5))
plt.errorbar(num_disconnected, throughput_ratio, yerr=std_throughput_ratio, fmt='-o', capsize=5, label='Throughput Ratio')

# Labels and title
plt.xlabel("Nodes Disconnected")
plt.ylabel("Throughput to Bandwidth Ratio (%)")
plt.title("Throughput Ratio vs Nodes Disconnected")
plt.legend()
plt.grid()

# Show plot
plt.show()
