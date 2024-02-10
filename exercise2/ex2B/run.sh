#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --time=01:00:00
#SBATCH --partition=EPYC
#SBATCH --job-name=HPC_ex2

module load openMPI/4.1.5/gnu/12.2.1
#export OMPI_MCA_pml=ucx

export OMP_NUM_THREADS=2
#export PSM3_MULTI_EP=1
mpirun -np 7 quick_mpi_omp 144

