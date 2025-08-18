import json, os, sys
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# Bajo cual agrupar
agrupar_por = 0

# Si le dan argumentos, agrupar bajo esos parámetros
if len(sys.argv) == 2:
	agrupar_por = int(sys.argv[1])

# Define parameters
first_params = ["5NN", "ER", "GG", "GPA", "RNG", "YAO"]
second_params = ["long", "square"]
third_params = ["imax3", "imax5", "imax7", "imax10"]

param_group = [first_params, second_params, third_params][agrupar_por]
xlabel = ["Topology Type", "Topology Shape", "Max Interlinks"][agrupar_por]

# Load JSON files
folder = "Correlations"  # Change this to your folder containing JSON files
all_data = []

for filename in os.listdir(folder):
    if filename.endswith(".json"):
        with open(os.path.join(folder, filename), "r") as f:
            data = json.load(f)
            # Extract parameters from filename
            params = filename.replace(".json", "").split("_")
            all_data.append((params, data))

# Organize data into DataFrames
def extract_correlations(correlation_type):
    # Elegir segun first, second o third, para agrupar segun eso. Tambien cambiar el valor de first_param=params[n]
    heatmap_data = {param: {} for param in param_group}

    for params, data in all_data:
        first_param = params[agrupar_por]   # Para elegir segun quien agrupar
        correlations = data[correlation_type]

        for metric, value in correlations.items():
            if metric not in heatmap_data[first_param]:
                heatmap_data[first_param][metric] = []
            heatmap_data[first_param][metric].append(value)

    # Convert to DataFrame (averaging values if needed)
    df = pd.DataFrame({k: {m: np.mean(v) for m, v in v.items()} for k, v in heatmap_data.items()})
    return df

# Plot heatmaps
def plot_heatmap(df, title):
    plt.figure(figsize=(12, 8))
    sns.heatmap(df, annot=True, cmap="coolwarm", center=0, linewidths=0.5)
    plt.title(title)
    plt.xlabel(xlabel)
    # plt.ylabel("Metric")
    plt.xticks(rotation=45)
    plt.yticks(rotation=0)
    plt.tight_layout()
    plt.show()

# Generate and plot heatmaps
for correlation_type in ["pearson", "spearman"]:
    df = extract_correlations(correlation_type)
    plot_heatmap(df, f"{correlation_type.capitalize()} Correlation Heatmap")