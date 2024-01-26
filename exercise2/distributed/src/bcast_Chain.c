//--------------------------------------------------------------------------------------
// Libraries
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
//--------------------------------------------------------------------------------------
// Broadcast function using chain algorithm
void bcast_Chain(void* data,int count,MPI_Datatype datatype,int root,MPI_Comm communicator){
  int world_rank;
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;
  MPI_Comm_size(communicator, &world_size);
  int tag=0; // I set the tag to 0 as default, but i think it could be any number

  if (world_rank == root){// If we are the root process, send our data to the next process
    MPI_Send(data, count, datatype, world_rank+1, 0, communicator);
  }
  else if (world_rank == world_size-1){// If we are the last process, receive the data from the previous process
    MPI_Recv(data, count, datatype, world_rank-1, 0, communicator, MPI_STATUS_IGNORE);
  }
  else{// If we are a receiver process, receive the data from the previous process and send it to the next one
    MPI_Recv(data, count, datatype, world_rank-1, 0, communicator, MPI_STATUS_IGNORE);
    MPI_Send(data, count, datatype, world_rank+1, 0, communicator);
  }
}
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
    bcast_Chain(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all processes before stopping the timer
    double end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    total_time += elapsed_time;
  }

  double average_time = total_time / num_measurements * 1000000; // Time in microseconds

  if (world_rank == 0) {
      printf("%f\n", average_time);
  }

  MPI_Finalize();

  return 0;
}
//--------------------------------------------------------------------------------------
