#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=24
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --job-name=HPC_exam
#SBATCH --exclusive
#SBATCH --exclude=fat[001-002]

# Load the openMPI module
module load openMPI/4.1.5/gnu

# Define the range of np values
np_values=$(seq 2 2 48)
# Define the range of map values
map_values="core socket node"

# Define filepaths
src_path="../../osu-micro-benchmarks-7.3/c/mpi/collective/blocking/"
out_csv="../barrier_results/barrier_tree.csv"

# Create the CSV file with header
echo "Algorithm,Allocation,Processes,Latency" > $out_csv

# Iterate over map and np values
for map in $map_values; do
  for np in $np_values; do
    # Run the mpirun command
    echo "...Benchmarking Tree Barrier with map=$map and np=$np..."
    mpirun -np $np -map-by $map --mca coll_tuned_use_dynamic_rules true --mca coll_tuned_barrier_algorithm 6 ${src_path}osu_barrier -x 1000 -i 10000 | tail -n 1 \
    | awk -v map="$map" -v np="$np" '{printf "Tree,%s,%s,%s\n",map,np,$1}' >> $out_csv
  done
done
