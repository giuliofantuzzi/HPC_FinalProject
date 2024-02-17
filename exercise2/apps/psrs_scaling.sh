#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=01:00:00
##SBATCH --nodelist=epyc[007]
#SBATCH --output=mpi_pure_scaling.out
#--------------------------------------------------------------------------------
csv="psrs_scaling.csv"
exe="./main_psrs.x"
N=1000000
OMP_threads=4
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1
#--------------------------------------------------------------------------------
echo "Processes,Threads,Time" > $csv 

# Step 1: serial benchmark
#export OMP_NUM_THREADS=1
#for iter in {1..3..1}
#do
#$exe $N >> $csv
#done
#--------------------------------------------------------------------------------
# Step 2: hybrid benchmark
export OMP_NUM_THREADS=$OMP_threads
for p in 2 4 8 16 32 
do
    for iter in {1..3..1}
    do
        mpirun -np $p --map-by socket $exe $N >> $csv
    done
done
#--------------------------------------------------------------------------------
