import json
import os
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# Define parameters
first_params = ["5NN", "ER", "GG", "GPA", "RNG", "YAO"]
second_params = ["long", "square"]
third_params = ["imax3", "imax5", "imax7", "imax10"]

# Load JSON files
folder = "Correlations"  # Change this to your folder containing JSON files
all_data = []

for filename in os.listdir(folder):
    if filename.endswith(".json"):
        with open(os.path.join(folder, filename), "r") as f:
            data = json.load(f)
            # Extract parameters from filename (modify if needed)
            params = filename.replace(".json", "").split("_")  # Assuming filenames follow a structured pattern
            all_data.append((params, data))

# Organize data into DataFrames
def extract_correlations(correlation_type):
    # Elegir segun first, second o third, para agrupar segun eso. Tambien cambiar el valor de first_param=params[n]
    heatmap_data = {param: {} for param in first_params}

    for params, data in all_data:
        first_param = params[0]   # Para elegir segun quien agrupar
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
    plt.figure(figsize=(10, 6))
    sns.heatmap(df, annot=True, cmap="coolwarm", center=0, linewidths=0.5)
    plt.title(title)
    plt.xlabel("Topology Type")
    plt.ylabel("Metric")
    plt.xticks(rotation=45)
    plt.yticks(rotation=0)
    plt.tight_layout()
    plt.show()

# Generate and plot heatmaps
for correlation_type in ["pearson", "spearman"]:
    df = extract_correlations(correlation_type)
    plot_heatmap(df, f"{correlation_type.capitalize()} Correlation Heatmap")