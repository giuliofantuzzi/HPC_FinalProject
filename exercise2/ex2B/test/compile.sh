#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPIcompile"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[007]


# Load modules and architecture
module load architecture/AMD
module load openMPI/4.1.5/gnu/
# Compile the code
mpicc -fopenmp source/quick_mpi.c main.c -o quick_mpi

