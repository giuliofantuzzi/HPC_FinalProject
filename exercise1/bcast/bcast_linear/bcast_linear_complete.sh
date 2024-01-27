#!/bin/bash
# Define the range of np values
np_values="2 4 8 16 32 64 128 256"
# Define the range of map values
map_values="core socket node"
#out csv
out_csv="../bcast_results/bcast_linear_complete.csv"

# Create the CSV file with header
echo "Algorithm,Allocation,Processes,MessageSize,Latency" > $out_csv

# Iterate over map and np values
for map in $map_values; do
 for np in $np_values; do
    # Run the mpirun command
    echo "...Benchmarking with map=$map and np=$np..."
    mpirun -np $np -map-by $map --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm 1 osu_bcast -x 100 -i 100 | tail -n 21 \
    | awk -v np="$np" -v map="$map" '{printf "Linear,%s,%s,%s,%s,%s\n",map,np,$1, $2, $3}' | sed 's/,$//' >> $out_csv
 done
done
