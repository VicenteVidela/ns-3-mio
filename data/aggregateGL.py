import os, sys, glob, json, statistics
from collections import defaultdict

def aggregateGL(topologia, forma, imax):
	data_directory = f"../resultados/Topologia1/fraccion_GL/{topologia}/{forma}/imax{imax}/"

	# Dictionary to count occurrences of values in different keys
	value_to_keys = defaultdict(list)

	# Process each file
	for file_name in glob.glob(os.path.join(data_directory, '*.fraction')):
		with open(file_name, 'r') as f:
			data = json.load(f)
			for key, values in data.items():
				for value in values:
					value_to_keys[value].append(float(key))

	# Determine the median key for each value
	median_keys = {}
	for value, keys in value_to_keys.items():
		median_key = statistics.median_low(keys)
		median_keys[value] = median_key

	output_file = f'AggregatedData/{topologia}_{forma}_imax{imax}.fraction'

	with open(output_file, 'w') as f:
		f.write(json.dumps(median_keys, indent=4))


# Check if arguments are provided
if len(sys.argv) == 4:
	print("Usage: aggregateData.py <topologia> <forma> <imax>")
	topologia = sys.argv[1]
	forma = sys.argv[2]
	imax = sys.argv[3]
	aggregateGL(topologia, forma, imax)
# if
else:
	for topologia in ['5NN', 'ER', 'GG', 'GPA', 'RNG', 'YAO']:
		for forma in ['long', 'square']:
			for imax in [3,5,7,10]:
				aggregateGL(topologia, forma, imax)

