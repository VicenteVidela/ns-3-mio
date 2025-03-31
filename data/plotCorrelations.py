import os
import json
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from glob import glob

def load_json_files(directory):
	"""Load all JSON files from a directory and return a list of dictionaries."""
	files = glob(os.path.join(directory, "*.json"))
	data = []
	for file in files:
		with open(file, "r") as f:
			data.append(json.load(f))
	return data

def aggregate_correlations(data):
	"""Aggregate correlation values and compute standard deviations across all files."""
	metrics = data[0]['pearson'].keys()  # Assume all files have the same metrics
	correlations = {metric: {"pearson": [], "spearman": [], "kendall": []} for metric in metrics}

	for entry in data:
		for metric in metrics:
			correlations[metric]["pearson"].append(entry["pearson"][metric])
			correlations[metric]["spearman"].append(entry["spearman"][metric])
			correlations[metric]["kendall"].append(entry["kendall"][metric])

	# Compute the average correlation and standard deviation per metric
	stats = {metric: {corr: {"mean": np.mean(correlations[metric][corr]), "std": np.std(correlations[metric][corr])} for corr in correlations[metric]} for metric in metrics}

	return stats

def plot_heatmap(correlations):
	"""Plot a heatmap of the correlation values with standard deviation annotations."""
	metrics = list(correlations.keys())
	corr_types = ["pearson", "spearman", "kendall"]

	corr_matrix = np.array([[correlations[metric][corr]["mean"] for corr in corr_types] for metric in metrics])
	std_matrix = np.array([[correlations[metric][corr]["std"] for corr in corr_types] for metric in metrics])

	plt.figure(figsize=(16, 9))
	sns.heatmap(corr_matrix, annot=True, cmap="coolwarm", xticklabels=[corr.capitalize() for corr in corr_types], yticklabels=metrics, fmt=".3f", cbar=True, linewidths=.5, linecolor='black')

	# Annotate with standard deviations
	for i in range(len(metrics)):
		for j in range(len(corr_types)):
			plt.text(j + 0.5, i + .75, f"±{std_matrix[i, j]:.3f}", ha='center', va='bottom', fontsize=10, color='white')

	plt.title("Correlation Heatmap with Standard Deviations")
	plt.xlabel("Correlation Type")
	plt.ylabel("Metric")
	plt.tight_layout()
	plt.show()

def plot_radar_chart(correlations):
	"""Plot a radar chart comparing correlation types."""
	metrics = list(correlations.keys())
	corr_types = ["pearson", "spearman", "kendall"]
	angles = np.linspace(0, 2 * np.pi, len(metrics), endpoint=False).tolist()
	angles += angles[:1]  # Close the circle

	fig, ax = plt.subplots(figsize=(8, 8), subplot_kw={"polar": True})
	for corr_type, color in zip(corr_types, ["b", "g", "r"]):
		values = [correlations[metric][corr_type]["mean"] for metric in metrics]
		values += values[:1]  # Close the circle
		ax.plot(angles, values, label=corr_type.capitalize(), color=color)
		ax.fill(angles, values, alpha=0.25, color=color)

	ax.set_xticks(angles[:-1])
	ax.set_xticklabels(metrics, rotation=45, ha="right")
	ax.set_title("Radar Chart of Correlations")
	ax.legend()
	plt.show()

if __name__ == "__main__":
	directory = "./Correlations"  # Change this to your actual directory containing JSON files
	data = load_json_files(directory)
	correlations = aggregate_correlations(data)
	plot_heatmap(correlations)
	# plot_radar_chart(correlations)
