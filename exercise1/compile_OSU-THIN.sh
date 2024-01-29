#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=12
#SBATCH --time=00:30:00
#SBATCH --partition=THIN
#SBATCH --exclusive

cd osu-micro-benchmarks-7.3-THIN

./configure CC=/opt/programs/openMPI/4.1.5/bin/mpicc CXX=/opt/programs/openMPI/4.1.5/bin/mpicxx
make
make install
