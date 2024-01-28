#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --time=00:30:00
#SBATCH --partition=EPYC

cd osu-micro-benchmarks-7.3

./configure CC=/opt/programs/openMPI/4.1.5/bin/mpicc CXX=/opt/programs/openMPI/4.1.5/bin/mpicxx
make
make install
