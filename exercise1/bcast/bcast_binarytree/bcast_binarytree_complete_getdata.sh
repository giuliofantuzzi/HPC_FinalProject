#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=128
#SBATCH --time=02:00:00
#SBATCH --partition=EPYC
#SBATCH --job-name=HPC_exam

# Load the openMPI module
module load openMPI/4.1.5/gnu

# Define the range of np values
np_values="2 4 8 16 32 64 128 192 256"
# Define the range of map values
map_values="core socket node"

# Define filepaths
src_path="../../osu-micro-benchmarks-7.3/c/mpi/collective/blocking/"
out_csv="../bcast_results/bcast_binarytree_complete.csv"

# Create the CSV file with header
echo "Algorithm,Allocation,Processes,MessageSize,Latency" > $out_csv

# Iterate over map and np values
for map in $map_values; do
  for np in $np_values; do
    # Run the mpirun command
    echo "...Benchmarking Binarytree with map=$map and np=$np..."
    mpirun -np $np -map-by $map --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_bcast_algorithm 5 ${src_path}osu_bcast -x 1000 -i 10000 | tail -n 21 \
    | awk -v np="$np" -v map="$map" '{printf "BinaryTree,%s,%s,%s,%s\n",map,np,$1, $2}' | sed 's/,$//' >> $out_csv
  done
done
