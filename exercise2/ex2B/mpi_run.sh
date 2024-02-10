#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI-W_scalability"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[007]

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpicc -fopenmp quick_mpi.c -o quick_mpi

#export OMP_PLACES=cores
#export OMP_PROC_BIND=close
export OMP_NUM_THREADS=2

mpirun -np 5 ./quick_mpi 113
rm quick_mpi
