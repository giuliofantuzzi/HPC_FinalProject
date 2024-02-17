#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="HPC_ex2"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --output=MPI_weak.out
#--------------------------------------------------------------------------------
csv="MPI_weak_160M.csv"
exe="/u/dssc/gfantuzzi/HPC_FinalProject/exercise2/apps/main.x"
# Define the Workload per process
W=2500000
OMP_threads=4
module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1
#--------------------------------------------------------------------------------
echo "Processes,Threads,Time" > $csv 

# Step 1: serial benchmark
#export OMP_NUM_THREADS=1
#for iter in {1..5..1}
#do
#    $exe $N >> $csv
#done
#--------------------------------------------------------------------------------
# Step 2: hybrid benchmark
export OMP_NUM_THREADS=$OMP_threads
for p in {2..64..1}
do
    for iter in {1..5..1}
    do
	N=$((W*p))
        mpirun -np $p --map-by socket $exe $N >> $csv
    done
done
#--------------------------------------------------------------------------------
