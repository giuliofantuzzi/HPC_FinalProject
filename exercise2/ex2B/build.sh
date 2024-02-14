#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="EX2B_compilation"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --time=00:10:00
#SBATCH --output=build_out.out

module load architecture/AMD
module load openMPI/4.1.5/gnu

mkdir -p build
cmake -S . -B build/
make -C build/ 
