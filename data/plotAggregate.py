import pandas as pd
import matplotlib.pyplot as plt
import sys
import json
import numpy as np
from decimal import *

mode = 1 # 0=save 1=show

def plot_metric(df_expanded, GL_values, metric, std_metric, ylabel, title, topologia, forma, imax, filename_suffix, color):
	fig, ax_left = plt.subplots(figsize=(16, 9))
	fig.suptitle(f"{topologia} {forma} imax{imax}")

	ax_left.plot(df_expanded["Fraction Physical Disconnected"], df_expanded[metric], linestyle='-', label=metric, color=color)
	ax_left.fill_between(df_expanded["Fraction Physical Disconnected"],
												df_expanded[metric] - df_expanded[std_metric],
												df_expanded[metric] + df_expanded[std_metric],
												alpha=0.2, color=color)
	ax_left.set_title(title)
	ax_left.set_xlabel('(1-p)')
	ax_left.set_ylabel(ylabel)
	ax_left.set_xticks(np.arange(0, 1.1, 0.1))
	ax_left.tick_params(axis='y')
	ax_left.grid(True)

	ax_right = ax_left.twinx()
	ax_right.plot(df_expanded["Fraction Physical Disconnected"], GL_values, linestyle='-', color='r', label='G_L')
	ax_right.set_ylabel('G_L')
	ax_right.tick_params(axis='y')

	handles_left, labels_left = ax_left.get_legend_handles_labels()
	handles_right, labels_right = ax_right.get_legend_handles_labels()
	ax_left.legend(handles_left + handles_right, labels_left + labels_right, loc='upper right')

	plt.tight_layout()
	if mode:
		plt.show()
	else:
		plt.savefig(f"Plots/{topologia}_{forma}_imax{imax}_{filename_suffix}.png")
	plt.close()

def plot_data(topologia, forma, imax):
	file = f"AggregatedData/{topologia}_{forma}_imax{imax}.dat"
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
		frac_logical = round(row["Fraction Logical Connected"], 4)
		if frac_logical == 0:
			break
		matching_keys = [float(k) for k, v in fraction_data.items() if v == frac_logical]
		if matching_keys:
			repeat_count = len(matching_keys)
			expanded_rows = pd.DataFrame([row] * repeat_count)
			expanded_rows["Fraction Physical Disconnected"] = matching_keys
			GL_values.extend([frac_logical] * repeat_count)
			df_expanded = pd.concat([df_expanded, expanded_rows], ignore_index=True)

	df_expanded.sort_values("Fraction Physical Disconnected", inplace=True)

	# Plot each metric
	plot_metric(df_expanded, GL_values, "Max Available Bandwidth (Kbps)", "Std Max Bandwidth",
							"Máximo Ancho de Banda (Kbps)", "Máximo Ancho de Banda", topologia, forma, imax, "bandwidth", 'blue')
	plot_metric(df_expanded, GL_values, "Throughput to Bandwidth Ratio (%)", "Std Throughput Ratio",
							"Porcentaje (%)", "Relación Throughput a Ancho de Banda", topologia, forma, imax, "throughput_ratio", 'purple')
	plot_metric(df_expanded, GL_values, "Packet Loss Percentage (%)", "Std Packet Loss",
							"Porcentaje (%)", "Porcentaje de Pérdida de Paquetes", topologia, forma, imax, "packet_loss", 'orange')
	plot_metric(df_expanded, GL_values, "Average Delay (ms)", "Std Delay",
							"Latencia (ms)", "Latencia Promedio", topologia, forma, imax, "delay", 'navy')
	plot_metric(df_expanded, GL_values, "Average Jitter (ms)", "Std Jitter",
							"Jitter (ms)", "Jitter Promedio", topologia, forma, imax, "jitter", 'magenta')

	print(f"{topologia}_{forma}_imax{imax} listoo")


# Si le dan argumentos, plotear ese caso
if len(sys.argv) == 4:
	topologia = sys.argv[1]
	forma = sys.argv[2]
	imax = sys.argv[3]

	plot_data(topologia, forma, imax)
# Si no, plotear todos los casos
else:
	for topologia in ['5NN', 'ER', 'GG', 'GPA', 'RNG', 'YAO']:
		for forma in ['long', 'square']:
			for imax in [3,5,7,10]:
				plot_data(topologia, forma, imax)