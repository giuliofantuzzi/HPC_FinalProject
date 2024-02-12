#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="omp_scalability"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[007]

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpicc -fopenmp quick.c -o quick_omp

#export OMP_PLACES=cores
#export OMP_PROC_BIND=close

for t in 256 400
do 
    echo "---------------------------"
    echo "OMP results for $t threads"
    export OMP_NUM_THREADS=$t
    ./quick_omp 1000000
    echo "----------------------------"
done

rm quick_omp
