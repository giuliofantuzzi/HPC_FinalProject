#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI-W_scalability"
#SBATCH --get-user-env
#SBATCH --partition=THIN
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=thin[010]

module load architecture/Intel
module load openMPI/4.1.4/gnu/12.2.1

mpicc -fopenmp try.c -o try.x

export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=2

mpirun -np 4 try.x 100

