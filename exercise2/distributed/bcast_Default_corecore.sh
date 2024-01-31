#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --time=01:00:00
#SBATCH --partition=THIN
#SBATCH --job-name=HPC_exam
#SBATCH --exclusive

module load openMPI/4.1.5/gnu

# Compile the C program
src=src/bcast_Default.c
exe=exe/bcast_Default_corecore
out_csv=results/corecore/bcast_Default_corecore.csv

mkdir exe
touch $out_csv
srun mpicc $src -o $exe

# Create a CSV file and add a header
echo "Algorithm,Processes,AvgTime" > $out_csv

# Loop over different numbers of processes
for processes in {2..36..1}; do
    echo "Benchmarking Chain with np=$processes..."
    # Run the MPI program with mpirun and save the average time to the CSV file
    result=$(mpirun -np $processes --map-by ppr:1:core:PE=1 --bind-to core $exe)
    echo "Default_corecore,$processes,$result" >> $out_csv
done

#Remove the executable directory
rm -r exe
