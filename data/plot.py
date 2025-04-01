import sys, json, os
import matplotlib.pyplot as plt
import numpy as np

# py plot.py 5NN long 5 34

# topologias = ['5NN','ER','GG','GPA','RNG','YAO']
# formas = ['long', 'square']
# imaxes = [3, 5, 7, 10]
# n van del 1 al 50

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 5:
  print("Usage: plot.py <topologia> <forma> <imax> <n>")
  sys.exit(1)

topologia = sys.argv[1]
forma = sys.argv[2]
imax = sys.argv[3]
n = sys.argv[4]

n_forma = '20x500' if (forma=='long') else '100x100'


file_name_string = f"../resultados/Topologia1/ataques/{topologia}/{forma}/imax{imax}/AV_it50_result_ppv3_lv1_{n_forma}_exp_2.5_ndep_{imax}_att_physical_v10_m_{topologia}_n{n}.dat"
file_name_physical_fraction_and_GL_string = f"../resultados/Topologia1/fraccion_GL/{topologia}/{forma}/imax{imax}/AV_it50_result_ppv3_lv1_{n_forma}_exp_2.5_ndep_{imax}_att_physical_v10_m_{topologia}_n{n}.fraction"

# Get the input file name from the command-line argument
# file_name = sys.argv[1]
# file_name_physical_fraction_and_GL = os.path.splitext(file_name.replace('ataques', 'fraccion_GL'))[0] + '.fraction'

# Open files for reading
with open(file_name_string, 'r') as f:
  data = f.read()
with open(file_name_physical_fraction_and_GL_string, 'r') as f:
  data_physical_fraction_and_GL = json.loads(f.read())

# Split the data into individual entries
entries = data.strip().split('\n\n')

# Initialize lists to store values
nodes_disconnected = []
max_bandwidth = []
throughput = []
throughput_ratio = []
packet_loss = []
mean_delay = []
mean_jitter = []
G_L = []

# List to store all fractions as an array
physical_fractions = []
corresponding_G_L = {}
for G_L_value, fraction_list in data_physical_fraction_and_GL.items():
  if G_L_value == '0.0':
    break
  physical_fractions.extend(fraction_list)
  corresponding_G_L[G_L_value] = len(fraction_list)

noMuerto = True
# Parse each entry and extract values
for entry in entries:
  lines = entry.split('\n')
  lines.pop(0)
  this_G_L = float(lines[7].split(": ")[1].split()[0])
  if (this_G_L == 0): break
  t = float(lines[2].split(": ")[1].split()[0])
  if t > 0:
    for i in range(corresponding_G_L[str(this_G_L)]):
      max_bandwidth.append(float(lines[1].split(": ")[1].split()[0]))
      throughput.append(t)
      throughput_ratio.append(float(lines[3].split(": ")[1])*100)
      packet_loss.append(float(lines[4].split(": ")[1][:-1]))
      mean_delay.append(float(lines[5].split(": ")[1].split()[0]))
      mean_jitter.append(float(lines[6].split(": ")[1].split()[0]))
      G_L.append(this_G_L)




# Helper function to combine legends for ax_left and ax_right
def combine_legends(ax_left, ax_right, loc):
  handles_left, labels_left = ax_left.get_legend_handles_labels()
  handles_right, labels_right = ax_right.get_legend_handles_labels()
  ax_left.legend(handles_left + handles_right, labels_left + labels_right, loc=loc)



# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))

fig.figure.suptitle(f"{topologia} {n_forma} imax{imax}, n: {n}")

x_ticks = np.arange(0, 1.1, 0.2)

# Plot Throughput Ratio
ax_left = axs[0, 0]
ax_left.plot(physical_fractions, throughput_ratio, linestyle='-', color='b', label='Throughput Ratio')
ax_left.set_title('Throughput to Bandwidth Ratio')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Ratio (%)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')

ax_right = ax_left.twinx()
ax_right.plot(physical_fractions, G_L, linestyle='-', color='r', label='${G_L}$')
ax_right.set_ylabel('${G_L}$')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')




# Plot Packet Loss Percentage
ax_left = axs[0, 1]
ax_left.plot(physical_fractions, packet_loss, linestyle='-', color='g', label='Packet Loss')
ax_left.set_title('Packet Loss Percentage')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Packet Loss (%)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')

ax_right = ax_left.twinx()
ax_right.plot(physical_fractions, G_L, linestyle='-', color='r', label='${G_L}$')
ax_right.set_ylabel('${G_L}$')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')





# Plot Max Bandwidth
ax_left = axs[1, 0]
ax_left.plot(physical_fractions, max_bandwidth, linestyle='-', color='purple', label='Max Bandwidth')
ax_left.set_title('Max Bandwidth')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Bandwidth (Kbps)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')

ax_right = ax_left.twinx()
ax_right.plot(physical_fractions, G_L, linestyle='-', color='r', label='${G_L}$')
ax_right.set_ylabel('${G_L}$')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')





# Combined Plot for Mean Delay and Mean Jitter
ax_left = axs[1, 1]
ax_left.plot(physical_fractions, mean_delay, linestyle='-', color='y', label='Mean Delay')
ax_left.plot(physical_fractions, mean_jitter, linestyle='-', color='m', label='Mean Jitter')
ax_left.set_title('Mean Delay and Mean Jitter')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Time (ms)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')

ax_right = ax_left.twinx()
ax_right.plot(physical_fractions, G_L, linestyle='-', color='r', label='${G_L}$')
ax_right.set_ylabel('${G_L}$')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')


plt.tight_layout()
plt.show()
