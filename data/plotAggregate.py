import pandas as pd
import matplotlib.pyplot as plt
import sys
import json
import numpy as np
from decimal import *

# Setear el numero de decimales a los datos de ivana
getcontext().prec = 4

# Verificar que se pasó un argumento de archivo
if len(sys.argv) < 2:
	print("Por favor, proporcione el nombre del archivo como argumento.")
	sys.exit(1)

# Nombre del archivo a partir de sys.argv
file = sys.argv[1]
file_fraction = file.replace('.dat', '.fraction')

# Columnas esperadas en el archivo
columns = [
	"Nodes Disconnected", "Max Available Bandwidth (Kbps)", "Std Max Bandwidth",
	"Average Throughput (Kbps)", "Std Throughput", "Throughput to Bandwidth Ratio (%)",
	"Std Throughput Ratio", "Packet Loss Percentage (%)", "Std Packet Loss",
	"Average Delay (ms)", "Std Delay", "Average Jitter (ms)", "Std Jitter",
]

# Leer y procesar los datos
df = pd.read_csv(file, skiprows=1, names=columns)

# Convertir las columnas necesarias a float
df["Nodes Disconnected"] = pd.to_numeric(df["Nodes Disconnected"], errors='coerce')
for col in ["Throughput to Bandwidth Ratio (%)", "Std Throughput Ratio"]:
	df[col] = pd.to_numeric(df[col], errors='coerce')


# Leer el archivo de fracción
with open(file_fraction, 'r') as f:
	fraction_data = json.load(f)

# Calcular la fracción de nodos conectados
total_nodes = 300
# Crear una nueva columna con la fracción de nodos logicos desconectados
df["Fraction Logical Connected"] = (total_nodes - df["Nodes Disconnected"]) / total_nodes

# valores de GL
GL_values = []



# Expandir el DataFrame para alinear con los valores de la fracción lógica
df_expanded = pd.DataFrame()
for _, row in df.iterrows():
	frac_logical = round(row["Fraction Logical Connected"],4)
	if frac_logical==0:
		break
	matching_keys = [float(k) for k, v in fraction_data.items() if v == frac_logical]
	if matching_keys:
		repeat_count = len(matching_keys)
		expanded_rows = pd.DataFrame([row] * repeat_count)
		expanded_rows["Fraction Physical Disconnected"] = matching_keys
		GL_values.extend([frac_logical]*repeat_count)
		df_expanded = pd.concat([df_expanded, expanded_rows], ignore_index=True)

df_expanded.sort_values("Fraction Physical Disconnected", inplace=True)

# Helper function to combine legends for ax_left and ax_right
def combine_legends(ax_left, ax_right, loc):
  handles_left, labels_left = ax_left.get_legend_handles_labels()
  handles_right, labels_right = ax_right.get_legend_handles_labels()
  ax_left.legend(handles_left + handles_right, labels_left + labels_right, loc=loc)



# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 12))
# fig.figure.suptitle(f"{topologia} {n_forma} imax{imax}, n: {n}")
fig.figure.suptitle(file.split('/')[1].replace('.dat', ''))
x_ticks = np.arange(0, 1.1, 0.2)



# Plot Max Available Bandwidth
ax_left = axs[0, 0]
ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded["Max Available Bandwidth (Kbps)"], linestyle='-', color='b', label='Bandwidth')
ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
				 df_expanded["Max Available Bandwidth (Kbps)"] - df_expanded["Std Max Bandwidth"],
				 df_expanded["Max Available Bandwidth (Kbps)"] + df_expanded["Std Max Bandwidth"],
				 alpha=0.2, color="b")
ax_left.set_title('Max Available Bandwidth')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Max Available Bandwidth (Kbps)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')
ax_left.grid(True)

ax_right = ax_left.twinx()
ax_right.plot(df_expanded["Fraction Physical Disconnected"], GL_values, linestyle='-', color='r', label='G_L')
ax_right.set_ylabel('G_L')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')

# Plot Throughput Ratio
ax_left = axs[0, 1]
ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded["Throughput to Bandwidth Ratio (%)"], linestyle='-', color='purple', label='Throughput Ratio')
ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
				 df_expanded["Throughput to Bandwidth Ratio (%)"] - df_expanded["Std Throughput Ratio"],
				 df_expanded["Throughput to Bandwidth Ratio (%)"] + df_expanded["Std Throughput Ratio"],
				 alpha=0.2, color="purple")
ax_left.set_title('Throughput to Bandwidth Ratio')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Ratio (%)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')
ax_left.grid(True)

ax_right = ax_left.twinx()
ax_right.plot(df_expanded["Fraction Physical Disconnected"], GL_values, linestyle='-', color='r', label='G_L')
ax_right.set_ylabel('G_L')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')

# Plot Packet Loss Percentage
ax_left = axs[1, 0]
ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded["Packet Loss Percentage (%)"], linestyle='-', color='orange', label='Packet Loss')
ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
				 df_expanded["Packet Loss Percentage (%)"] - df_expanded["Std Packet Loss"],
				 df_expanded["Packet Loss Percentage (%)"] + df_expanded["Std Packet Loss"],
				 alpha=0.2, color="orange")
ax_left.set_title('Packet Loss Percentage (%)')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Packet Loss Percentage (%)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')
ax_left.grid(True)

ax_right = ax_left.twinx()
ax_right.plot(df_expanded["Fraction Physical Disconnected"], GL_values, linestyle='-', color='r', label='G_L')
ax_right.set_ylabel('G_L')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')


# Plot Packet Loss Percentage
ax_left = axs[1, 1]
ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded["Average Delay (ms)"], linestyle='-', color='y', label='Delay')
ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
				 df_expanded["Average Delay (ms)"] - df_expanded["Std Delay"],
				 df_expanded["Average Delay (ms)"] + df_expanded["Std Delay"],
				 alpha=0.2, color="orange")
ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded["Average Jitter (ms)"], linestyle='-', color='m', label='Jitter')
ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
				 df_expanded["Average Jitter (ms)"] - df_expanded["Std Jitter"],
				 df_expanded["Average Jitter (ms)"] + df_expanded["Std Jitter"],
				 alpha=0.2, color="orange")
ax_left.set_title('Average Delay and Average Jitter')
ax_left.set_xlabel('(1-p)')
ax_left.set_ylabel('Delay (ms) / Jitter (ms)')
ax_left.set_xticks(x_ticks)
ax_left.tick_params(axis='y')
ax_left.grid(True)

ax_right = ax_left.twinx()
ax_right.plot(df_expanded["Fraction Physical Disconnected"], GL_values, linestyle='-', color='r', label='G_L')
ax_right.set_ylabel('G_L')
ax_right.tick_params(axis='y')

combine_legends(ax_left, ax_right, loc='upper right')


# Mostrar las gráficas
plt.tight_layout()
plt.show()