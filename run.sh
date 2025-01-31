#!/bin/bash

# Define variables
BASE_DIR="scratch/topologies/"
TOPOLOGY_DIR="ER_50_iteraciones/"
TOPOLOGY_FILE="logic_exp_2.5_v1.csv"
OUTPUT_DIR="resultados"

# Get a list of all .disc files in the specific directory and its subdirectories
DISCONNECTIONS_FILES=$(find "${BASE_DIR}${TOPOLOGY_DIR}" -type f -name "*.disc")

# Loop over each disconnections file
for DISCONNECTIONS_FILE in $DISCONNECTIONS_FILES; do
  # Extract the relative path of the file (from the base directory onwards)
  RELATIVE_PATH="${DISCONNECTIONS_FILE#${BASE_DIR}}"

  # Get the subdirectory path (excluding the filename)
  SUBDIR=$(dirname "$RELATIVE_PATH")

  # Create the corresponding subdirectory inside the output directory
  mkdir -p "${OUTPUT_DIR}/${SUBDIR}"

  # Extract just the filename from the full path
  FILENAME=$(basename "$DISCONNECTIONS_FILE")

  # Extract only the inner arrays and count them
  ARRAY_LENGTH=$(( $(grep -o '\[[^]]*\]' "$DISCONNECTIONS_FILE" | wc -l) - 1))

  echo "Corriendo $FILENAME"

  # Loop over iterations based on the array length
  for i in $(seq 0 $((ARRAY_LENGTH))); do
    echo "Corriendo iteracion $i de $ARRAY_LENGTH"

    # Define the output file path, preserving the subdirectory structure
    OUTPUT_FILE="${OUTPUT_DIR}/${SUBDIR}/${FILENAME%.disc}.dat"

    # Run the ns-3 command
    ./ns3 run "5Desconexiones \
      --specificTopologyDirectory=${TOPOLOGY_DIR} \
      --topology=${TOPOLOGY_FILE} \
      --disconnectionsFile=${DISCONNECTIONS_FILE} \
      --output=${OUTPUT_FILE} \
      --iteration=${i}"

    # Check if the command failed
    if [ $? -ne 0 ]; then
      echo "Run failed for file $FILENAME, iteration $i. Skipping to next file..."
      break  # Skip to the next file
    fi
  done
done
