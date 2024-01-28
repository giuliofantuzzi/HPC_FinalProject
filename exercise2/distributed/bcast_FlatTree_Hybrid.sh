#!/bin/bash

src=src/bcast_FlatTree_Hybrid.c
exe=exe/bcast_FlatTree_Hybrid
out_csv=results/bcast_FlatTree_Hybrid.csv

mkdir -p exe
touch $out_csv

# Compiler flags for OpenMP
omp_flags="-fopenmp"

# Compile the hybrid MPI-OpenMP program
srun mpicc $src -o $exe $omp_flags

# Create a CSV file and add a header
echo "Algorithm,Processes,Threads,AvgTime" > $out_csv

# Loop over different numbers of processes
for processes in {2..128..1}; do
    # Loop over different numbers of OpenMP threads
    for threads in {2..8..2}; do
        echo "Benchmarking Hybrid with np=$processes, threads=$threads..."
        
        # Redirect the output of your C program to the CSV file
        mpirun -np $processes --map-by core $exe OMP_NUM_THREADS=$threads >> $out_csv
    done
done

# Remove the executable directory
rm -r exe
