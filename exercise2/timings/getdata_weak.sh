#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --output=WeakScalability.out
#--------------------------------------------------------------------------------
# Global settings
csv=$1
exe="../apps/main.x"
W=2500000
OMP_threads=4
#--------------------------------------------------------------------------------
# Load modules
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1
#--------------------------------------------------------------------------------
echo "Processes,Threads,Time" > $csv 
# Step 1: benchmark serial version (1 process 1 thread)
export OMP_NUM_THREADS=1
for iter in {1..5..1}
do
   $exe $W >> $csv
done
#--------------------------------------------------------------------------------
# Step 2: benchmark hybrid version
export OMP_NUM_THREADS=$OMP_threads
for p in {1..128..1}
do
    N=$((W*p))
    for iter in {1..5..1}
    do
        mpirun -np $p --map-by socket $exe $N >> $csv
    done
done
#--------------------------------------------------------------------------------
