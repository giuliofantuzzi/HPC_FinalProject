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

#mpicc -fopenmp quick_mpi.c -o quick_mpi
#gcc quick.c -o quick
gcc quick.c -o quick

#export OMP_PLACES=cores
#export OMP_PROC_BIND=close
#export OMP_NUM_THREADS=1

./quick 100000
#./quick 100000
rm quick
