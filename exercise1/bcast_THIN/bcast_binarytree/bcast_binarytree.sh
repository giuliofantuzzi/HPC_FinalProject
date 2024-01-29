#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=12
#SBATCH --time=01:00:00
#SBATCH --partition=THIN
#SBATCH --job-name=HPC_exam
#SBATCH --exclusive
module load openMPI/4.1.5/gnu
# move inside the directory where to perform benchmarking
src_path="../../osu-micro-benchmarks-7.3-THIN/c/mpi/collective/blocking/"
# Specify the output CSV file and set its header
out_csv="../bcast_results/bcast_binarytree.csv"
echo "Algorithm,Processes,AvgLatency(us),MinLatency,MaxLatency" >> $out_csv

for np in {2..24..1}
do
    # Construct the mpirun command with the current values of np and algorithm
    command="mpirun -np $np --map-by core --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm 5 ${src_path}osu_bcast -x 1000 -i 10000 -m 1:1 -f"
    # Execute the command, extract the numbers and append to the CSV file
    echo "Currentlty benchmarking BinaryTree with n. of processes= $np"
    latency_stats=$(eval $command | tail -n 1 | awk '{gsub(/ +/, ","); sub(/^[^,]+,/, ""); sub(/,[^,]+$/, ""); print}')
    echo "BinaryTree,$np,$latency_stats" >> $out_csv
done




