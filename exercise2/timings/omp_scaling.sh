#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:15:00
#SBATCH --output=omp_scaling.out

csv="omp_scaling_L1_640M.csv"
exe="/u/dssc/gfantuzzi/HPC_FinalProject/exercise2/apps/main.x"
N=640000000
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1
#--------------------------------------------------------------------------------
#echo "Processes,Threads,Time" > $csv 


for t in 1 2 4 8 16 32 64 
do
    export OMP_NUM_THREADS=$t
    for iter in {1..10..1}
    do
        $exe $N >> $csv
    done
done
#--------------------------------------------------------------------------------
