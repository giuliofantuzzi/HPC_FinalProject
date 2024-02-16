#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
##SBATCH --nodelist=epyc[007]
#SBATCH --output=mpi_pure_scaling.out

csv="../timings/hybrid_scaling_240M.csv"
exe="./main.x"
N=240000000
#OMP_threads=2
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1


echo "Processes,Threads,Time" > $csv 

# serial benchmark
export OMP_NUM_THREADS=1
for iter in {1..5..1}
do
    $exe $N >> $csv
done

# hybrid benchmark
export OMP_NUM_THREADS=4
for p in 2 4 8 16 32 64
do
    for iter in {1..5..1}
    do
        mpirun -np $p --map-by socket $exe $N >> $csv
    done
done
