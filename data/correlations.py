import pandas as pd
import sys
import json


def calculate_correlation(topologia, forma, imax):
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

  # Add GL_values to the expanded DataFrame
  df_expanded["GL_values"] = GL_values

  # Calculate correlations
  metrics = [
    "Max Available Bandwidth (Kbps)", "Average Throughput (Kbps)", "Throughput to Bandwidth Ratio (%)",
    "Packet Loss Percentage (%)", "Average Delay (ms)", "Average Jitter (ms)"
  ]

  correlations_pearson = {}
  correlations_spearman = {}
  correlations_kendall = {}

  for metric in metrics:
    correlation_pearson = df_expanded["GL_values"].corr(df_expanded[metric])
    correlations_pearson[metric] = correlation_pearson
    correlation_spearman = df_expanded["GL_values"].corr(df_expanded[metric], method='spearman')
    correlations_spearman[metric] = correlation_spearman
    correlation_kendall = df_expanded["GL_values"].corr(df_expanded[metric], method='kendall')
    correlations_kendall[metric] = correlation_kendall

  correlations = {
    'pearson': correlations_pearson,
    'spearman': correlations_spearman,
    'kendall': correlations_kendall
  }

  # Print correlations
  # for metric, correlation in correlations.items():
  #   print(f"{metric}: {correlation}")

  # Save correlations to a file
  output_file = f"Correlations/{topologia}_{forma}_imax{imax}_correlations.json"
  with open(output_file, 'w') as f:
    json.dump(correlations, f, indent=4)

  print(f"{topologia} {forma} imax{imax} listoo")


# Si le dan argumentos, calcular ese caso
if len(sys.argv) == 4:
  topologia = sys.argv[1]
  forma = sys.argv[2]
  imax = sys.argv[3]

  calculate_correlation(topologia, forma, imax)
# Si no, calcular todos los casos
else:
  for topologia in ['5NN', 'ER', 'GG', 'GPA', 'RNG', 'YAO']:
    for forma in ['long', 'square']:
      for imax in [3,5,7,10]:
        calculate_correlation(topologia, forma, imax)