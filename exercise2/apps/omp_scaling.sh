#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
##SBATCH --nodelist=epyc[007]
#SBATCH --output=omp_scaling.out

csv="../timings/omp_scaling_L1.csv"
exe="./main.x"
N=640000000
#OMP_threads=2
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1


#echo "Processes,Threads,Time" > $csv 

for iter in {1..2..1}
do
    for t in 1 2 4 8 16 32 64
    do
        export OMP_NUM_THREADS=$t
        $exe $N >> $csv
    done
done
