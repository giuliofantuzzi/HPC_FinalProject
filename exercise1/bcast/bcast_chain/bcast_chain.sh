#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=128
#SBATCH --time=02:00:00
#SBATCH --partition=EPYC
#SBATCH --job-name=HPC_exam

# move inside the directory where to perform benchmarking
src_path="../../osu-micro-benchmarks-7.3/c/mpi/collective/blocking/"
# Specify the output CSV file and set its header
out_csv="../bcast_results/bcast_chain.csv"
echo "Algorithm,Processes,AvgLatency(us),MinLatency,MaxLatency" >> $out_csv

for np in {2..256..1}
do
    # Construct the mpirun command with the current values of np and algorithm
    command="mpirun -np $np --map-by core --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm 2 ${src_path}osu_bcast -x 1000 -i 10000 -m 1:1 -f"
    # Execute the command, extract the numbers and append to the CSV file
    echo "Currentlty benchmarking Chain with n. of processes= $np"
    latency_stats=$(eval $command | tail -n 1 | awk '{gsub(/ +/, ","); sub(/^[^,]+,/, ""); sub(/,[^,]+$/, ""); print}')
    echo "Chain,$np,$latency_stats" >> $out_csv
done

