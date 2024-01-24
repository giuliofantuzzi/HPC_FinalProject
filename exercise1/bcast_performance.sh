#!/bin/bash
# move inside the directory where to perform benchmarking
cd osu-micro-benchmarks-7.3/c/mpi/collective/blocking/
# Specify the CSV file
csv_file="../../../../../results/bcast_performance.csv"
echo "Algorithm,Processes,Avg Latency(us)" >> $csv_file
# Specify the list of algorithms
algorithms=(1 2 5)

# Iterate over algorithms
for algorithm in "${algorithms[@]}"
do
    echo "Benchmarking for algorithm $algorithm"
    for np in {2..128..1}
    do
      # Construct the mpirun command with the current values of np and algorithm
      command="mpirun -np $np --map-by core --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm $algorithm osu_bcast -m 1"
      # Execute the command, extract the rightmost number from the last line, and append to the CSV file
      echo "N.of processes: $np"
      latency=$(eval $command | tail -n 1 | awk '{print $NF}')
      echo "$algorithm,$np,$latency" >> $csv_file
    done
done


