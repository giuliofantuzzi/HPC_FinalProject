#!/bin/bash
# move inside the directory where to perform benchmarking
exe_filepath=../../osu-micro-benchmarks-7.3/c/mpi/collective/blocking/

# Specify the output CSV file and set its header
csv_file="../bcast_results/bcast_standard.csv"
echo "Algorithm,Processes,AvgLatency(us),MinLatency,MaxLatency" >> $csv_file

for np in {2..256..1}
    do
    # Construct the mpirun command with the current values of np and algorithm
    command="mpirun -np $np --map-by core  ${exe_filepath}osu_bcast -x 100 -i 100 -m 1:1 -f"
    # Execute the command, extract the numbers and append to the CSV file
    echo "Currentlty benchmarking with n. of processes= $np"
    latency_stats=$(eval $command | tail -n 1 | awk '{gsub(/ +/, ","); sub(/^[^,]+,/, ""); sub(/,[^,]+$/, ""); print}')
    echo "standard,$np,$latency_stats" >> $csv_file
done


