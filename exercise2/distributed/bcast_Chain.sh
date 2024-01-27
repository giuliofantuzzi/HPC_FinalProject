#!/bin/bash
# Remember to alloc a node!
# Compile the C program
src=src/bcast_Chain.c
exe=exe/bcast_Chain
out_csv=results/bcast_Chain.csv

mkdir exe
touch $out_csv
srun mpicc $src -o $exe

# Create a CSV file and add a header
echo "Algorithm,Processes,AvgTime" > $out_csv

# Loop over different numbers of processes
for processes in {2..128..1}; do
    echo "Benchmarking Chain with np=$processes..."
    # Run the MPI program with mpirun and save the average time to the CSV file
    result=$(mpirun -np $processes --map-by core -x 1000 $exe)
    echo "Chain,$processes,$result" >> $out_csv
done

#Remove the executable directory
rm -r exe
