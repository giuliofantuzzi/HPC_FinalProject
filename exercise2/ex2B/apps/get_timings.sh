#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[001]
#SBATCH --output=out_times.out


exe="./main.x"
N=10000000
MPI_procs=16
OMP_threads=8
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

# 1) Run the serial version
#export OMP_NUM_THREADS=1
echo "Benchmarking serial version"
mpirun -np 1 $exe $N

# 2) Run the omp-only version
export OMP_NUM_THREADS=$OMP_threads
echo "Benchmarking omp version"
mpirun -np 1 $exe $N

# 3) Run the hybrid version (with the same number of threads)
echo "Benchmarking MPI version"
mpirun -np $MPI_procs --map-by socket $exe $N
