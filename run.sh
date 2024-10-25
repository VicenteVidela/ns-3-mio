#!/bin/bash

# Define variables
BASE_DIR="scratch/topologies/"
TOPOLOGY_DIR="Topologia1/"
TOPOLOGY_FILE="logic_exp_2.5_v1.csv"
OUTPUT_DIR="PrimeraTopologia"

# Get a list of all .disc files in the specific directory
DISCONNECTIONS_FILES=("${BASE_DIR}${TOPOLOGY_DIR}"*.disc)

# Loop over each disconnections file
for DISCONNECTIONS_FILE in "${DISCONNECTIONS_FILES[@]}"; do
  # Extract just the filename from the full path
  FILENAME=$(basename "$DISCONNECTIONS_FILE")

  # Extract only the inner arrays and count them
  ARRAY_LENGTH=$(grep -o '\[[^]]*\]' "$DISCONNECTIONS_FILE" | wc -l)

  echo "Corriendo $FILENAME"

  # Loop over iterations based on the array length
  for i in $(seq 0 $((ARRAY_LENGTH))); do
    echo "Corriendo iteracion $i de $ARRAY_LENGTH"

    OUTPUT_FILE="${OUTPUT_DIR}/${FILENAME%.disc}.dat"

    # Run the ns-3 command
    ./ns3 run "5Desconexiones \
      --specificTopologyDirectory=${TOPOLOGY_DIR} \
      --topology=${TOPOLOGY_FILE} \
      --disconnectionsFile=${FILENAME} \
      --output=${OUTPUT_FILE} \
      --iteration=${i}"

    # Check if the command failed
    if [ $? -ne 0 ]; then
      echo "Run failed for file $FILENAME, iteration $i. Skipping to next file..."
      break  # Skip to the next file
    fi
  done
done
