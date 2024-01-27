#!/bin/bash

# Define the range of np values
np_values="2 4 8 16 32 64 128 256"
# Define the range of map values
map_values="1 2 4"
#out csv
out_csv="../bcast_results/bcast_linear_complete.csv"

# Create the CSV file with header
echo "map,np,size,latency" > $out_csv

# Iterate over map and np values
for map in $map_values; do
  for np in $np_values; do
    # Run the mpirun command
    echo "...Benchmarking with map=$map and np=$np..."
    output=$(mpirun -np $np -map-by $map --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm 1 osu_bcast  -x 1000 -i 10000)
    # Extract and format the relevant information
    while read -r line; do
      if [[ $line =~ ^([0-9]+)\s+([0-9.]+)$ ]]; then
        size="${BASH_REMATCH[1]}"
        latency="${BASH_REMATCH[2]}"
        # Append the result to the CSV file
        echo "$map,$np,$size,$latency" >> $out_csv
      fi
    done <<< "$output"
  done
done
