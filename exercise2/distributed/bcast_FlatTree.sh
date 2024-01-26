#!/bin/bash
# Remember to alloc a node!
# Compile the C program
src=src/bcast_FlatTree.c
exe=exe/bcast_FlatTree
out_csv=results/bcast_FlatTree.csv

mkdir exe
touch $out_csv
srun mpicc $src -o $exe

# Create a CSV file and add a header
echo "Algorithm,Processes,AvgTime" > $out_csv

# Loop over different numbers of processes
for processes in {2..128..1}; do
    echo "Benchmarking Flat Tree with np=$processes..."
    # Run the MPI program with mpirun and save the average time to the CSV file
    result=$(mpirun -np $processes $exe)
    echo "FlatTree,$processes,$result" >> $out_csv
done

#Remove the executable directory
rm -r exe
