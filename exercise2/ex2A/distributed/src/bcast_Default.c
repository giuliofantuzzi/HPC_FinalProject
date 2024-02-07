//--------------------------------------------------------------------------------------
// Libraries
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
//--------------------------------------------------------------------------------------
int main(int argc, char** argv){

  MPI_Init(&argc, &argv);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int data = 42; 
  int num_measurements = 1000;  // Number of times to measure

  double total_time = 0.0;

  for (int i = 0; i < num_measurements; i++) {

    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all processes before starting the timer
    double start_time = MPI_Wtime();
    MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all processes before stopping the timer
    double end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    total_time += elapsed_time;
  }

  double average_time = total_time / num_measurements * 1000000; //result in microseconds!

  if (world_rank == 0) {
      printf("%f\n", average_time);
  }

  MPI_Finalize();

  return 0;
}
//--------------------------------------------------------------------------------------