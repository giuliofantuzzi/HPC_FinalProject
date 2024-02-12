#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[005]
# set the name of the out
#SBATCH --output=times_100000.out

N=100000
procs=16
export OMP_NUM_THREADS=4

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpicc quick_mpi.c -o quick_serial
mpicc -fopenmp quick_mpi.c -o quick_hybrid

# 1) Run the serial version
mpirun -np 1 ./quick_mpi $N
# 2) Run the omp-only version
mpirun -np 1 ./quick_mpi $N
# 3) Run the hybrid version (with the same number of threads)
mpirun -np $procs --map-by socket ./quick_mpi 100000

rm quick_serial quick_hybrid