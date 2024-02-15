#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[005]
#SBATCH --output=out_times.out

csv="../timings/timings_64M.csv"
exe="./main.x"
N=64000
MPI_procs=4
OMP_threads=2
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

echo "Processes,Threads,Time" > $csv 
# 1) Run the serial version
export OMP_NUM_THREADS=1
echo "Benchmarking serial version"
mpirun -np 1 $exe $N >> $csv

# 2) Run the omp-only version
export OMP_NUM_THREADS=$OMP_threads
echo "Benchmarking omp version"
mpirun -np 1 $exe $N >> $csv

# 3) Run the hybrid version (with the same number of threads)
echo "Benchmarking MPI version"
mpirun -np $MPI_procs --map-by socket $exe $N >> $csv
