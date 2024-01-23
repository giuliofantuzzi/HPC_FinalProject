//--------------------------------------------------------------------------------------
// Libraries
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
//--------------------------------------------------------------------------------------
// Custom funtion to implement broadcast using ONLY point-to-point communication
void my_bcast(void* data,int count,MPI_Datatype datatype,int root,MPI_Comm communicator)
{
  int world_rank;
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;
  MPI_Comm_size(communicator, &world_size);
  int tag=0; // I set the tag to 0 as default, but i think it could be any number

  if (world_rank == root)// If we are the root process, send our data to everyone else
  {
    int i;
    for (i = 0; i < world_size; i++)
    {
      if (i != world_rank)
      {
        MPI_Send(data, count, datatype, i, 0, communicator);
      }
    }
  }
  else // If we are a receiver process, receive the data from the root
  {
    MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
  }
}
//--------------------------------------------------------------------------------------
int main()
{
    return 0;
}
//--------------------------------------------------------------------------------------
