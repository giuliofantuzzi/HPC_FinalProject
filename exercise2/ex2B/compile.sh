#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --time=01:00:00
#SBATCH --partition=EPYC
#SBATCH --job-name=HPC_ex2
#SBATCH --exclusive
#SBATCH --exclude=fat[001-002]

module load openMPI/4.1.5/gnu/12.2.1
export OMPI_MCA_pml=ucx

#Compile MPI with omp
mpicc -fopenmp quick_mpi.c -o quick_mpi_omp
