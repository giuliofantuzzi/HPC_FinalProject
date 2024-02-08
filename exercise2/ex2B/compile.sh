#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --time=01:00:00
#SBATCH --partition=THIN
#SBATCH --job-name=HPC_ex2
#SBATCH --exclusive
#SBATCH --exclude=fat[001-002]

module load openMPI/4.1.5/gnu
export OMPI_MCA_pml=ucx

# 1) compile the serial version
gcc quick.c -o quick_serial
# 2) compile the omp version
gcc -fopenmp quick.c -o quick_omp

# 3) compile MPI without omp
mpicc quick_mpi.c -o quick_mpi

# 4) compile MPI with omp
mpicc -fopenmp quick_mpi.c -o quick_mpi_omp
