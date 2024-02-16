#include "quick_mpi.h"

void psrs(data_t** data, int* chunk_size, MPI_Datatype MPI_DATA_T, MPI_Comm comm, compare_t cmp_ge){
    // Function that implements the PSRS algorithm
    // where data is already divided into chunks

    int rank, num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);

    // Step 1: Sort the local arrays
    #pragma omp parallel
    {
        #pragma omp single
        par_quicksort(*data, 0, *chunk_size, cmp_ge);
    }

    // Step 2: Select the pivots
    //data_t* pivots = (data_t*)malloc(num_procs*sizeof(data_t));
    int* sample_indices = (int*)malloc(num_procs*sizeof(int));

    // Generate the sample indices
    int step = *chunk_size / num_procs;
    for (int i = 0; i < num_procs; i++){
        sample_indices[i] = i*step +1;
    }

    // Select the samples
    data_t* samples = (data_t*)malloc(num_procs*sizeof(data_t));
    for (int i = 0; i < num_procs; i++){
        samples[i] = (*data)[sample_indices[i]];
    }

    // Declare a data_t array to store the samples
    data_t* pivots = (data_t*)malloc(num_procs*num_procs*sizeof(data_t));
    // if (rank == 0){
    //     pivots = (data_t*)malloc(num_procs*num_procs*sizeof(data_t));
    // }

    MPI_Barrier(MPI_COMM_WORLD);
    // Gather the samples
    MPI_Gather(samples, num_procs, MPI_DATA_T, pivots, num_procs, MPI_DATA_T, 0, MPI_COMM_WORLD);

    data_t* selected_pivots = (data_t*)malloc((num_procs-1)*sizeof(data_t));
    if (rank == 0){
        printf("Pivots gathered sorting:\n");
        show_array(pivots, 0, num_procs*num_procs, 0);

        // Sort the pivots
        par_quicksort(pivots, 0, num_procs*num_procs, cmp_ge);


        printf("Pivots sorted:\n");
        show_array(pivots, 0, num_procs*num_procs, 0);

        // Select the pivots
        for (int i = 1; i < num_procs; i++){
            selected_pivots[i - 1] = pivots[i*num_procs];
        }

        printf("Selected pivots:\n");
        show_array(selected_pivots, 0, num_procs -1, 0);
    }

    // Broadcast the selected pivots from rank 0
    MPI_Bcast(selected_pivots, num_procs -1, MPI_DATA_T, 0, comm);

    for (int i = 0; i < num_procs; i++){
        if(rank == i){
        printf("Process %d has received\n", rank);
        show_array(selected_pivots, 0, num_procs -1, 0);
        }
        MPI_Barrier(comm);
    }

    // Step 3: Partition the local arrays
    int* pivot_positions = (int*)malloc(num_procs*sizeof(int));
    for (int i = 0; i < num_procs - 1; i++){
        pivot_positions[i] = mpi_partition(*data, 0, *chunk_size, cmp_ge, &selected_pivots[i]);
    }

    for (int i=0; i<num_procs; i++){
        if(rank == i){
            printf("Process %d has selected the pivot positions\n", rank);
            for (int j = 0; j < num_procs - 1; j++){
                printf("%d ", pivot_positions[j]);
            }
        }
        MPI_Barrier(comm);
    }

    // AllGather the chunk sizes and pivot positions
    int* all_chunk_sizes = (int*)malloc(num_procs*sizeof(int));
    int* all_pivot_positions = (int*)malloc((num_procs * (num_procs - 1))*sizeof(int));
    MPI_Allgather(chunk_size, 1, MPI_INT, all_chunk_sizes, 1, MPI_INT, comm);
    MPI_Allgather(pivot_positions, num_procs - 1, MPI_INT, all_pivot_positions, num_procs - 1, MPI_INT, comm);

    printf("Process %d has gathered the chunk sizes and pivot positions\n", rank);
    for (int i = 0; i < num_procs; i++){
        if (rank == i){
            printf("All chunk sizes:\n");
            for (int j = 0; j < num_procs; j++){
                printf("%d ", all_chunk_sizes[j]);
            }
            printf("\n");
            printf("All pivot positions:\n");
            for (int j = 0; j < num_procs * (num_procs - 1); j++){
                printf("%d ", all_pivot_positions[j]);
            }
            printf("\n");
        }
        MPI_Barrier(comm);
    }

    // Initialize sendcounts and sdispls
    int* sendcounts = (int*)malloc(num_procs*sizeof(int));
    int* sdispls = (int*)malloc(num_procs*sizeof(int));
    int* recvcounts = (int*)malloc(num_procs*sizeof(int));
    int* rdispls = (int*)malloc(num_procs*sizeof(int));
    for (int i = 0; i < num_procs; i++){
        //sendcounts[i] = i == 0 ? pivot_positions[i] + 1 : pivot_positions[i] - pivot_positions[i - 1];
        // Set sendcounts
        if (i == 0){
                sendcounts[i] = pivot_positions[i] + 1;
        }else if (i == num_procs - 1){
                sendcounts[i] = *chunk_size - 1 - pivot_positions[i - 1];
        }else{
                sendcounts[i] = pivot_positions[i] - pivot_positions[i - 1];
        }

        //Set receive counts
        //recvcounts[i] = all_chunk_sizes[(i*num_procs) + rank];

        if (rank ==0){
                recvcounts[i] = all_pivot_positions[(i*(num_procs-1))] + 1;
        }else if (rank == num_procs - 1){
                recvcounts[i] = all_chunk_sizes[i] - 1 - all_pivot_positions[(i*(num_procs-1)) + rank - 1];
        }else{
                recvcounts[i] = all_pivot_positions[(i*(num_procs-1)) + rank] - all_pivot_positions[(i*(num_procs-1)) + rank - 1];
        }

        // Set send displacements
        sdispls[i] = 0;
        for (int j = 0; j < i; j++){
            sdispls[i] += sendcounts[j];
        }
        
        // Set receive displacements
        rdispls[i] = 0;
        for (int j = 0; j < i; j++){
            rdispls[i] += recvcounts[j];
        }
    }

    MPI_Barrier(comm);

    printf("Process %d has set the send and receive counts and displacements\n", rank);
    for (int i = 0; i < num_procs; i++){
        if (rank == i){
            printf("Send counts:\n");
            for (int j = 0; j < num_procs; j++){
                printf("%d ", sendcounts[j]);
            }
            printf("\n");
            printf("Receive counts:\n");
            for (int j = 0; j < num_procs; j++){
                printf("%d ", recvcounts[j]);
            }
            printf("\n");
            printf("Send displacements:\n");
            for (int j = 0; j < num_procs; j++){
                printf("%d ", sdispls[j]);
            }
            printf("\n");
            printf("Receive displacements:\n");
            for (int j = 0; j < num_procs; j++){
                printf("%d ", rdispls[j]);
            }
            printf("\n");
        }
        MPI_Barrier(comm);
    }

    // Initialize the merged array
    int new_chunk_size = 0;
    for (int i = 0; i < num_procs; i++){
        new_chunk_size += recvcounts[i];
    }
    data_t* merged = (data_t*)malloc(new_chunk_size*sizeof(data_t));

    printf("New array size is %d\n", new_chunk_size);

    // Alltoallv
    MPI_Alltoallv(*data, sendcounts, sdispls, MPI_DATA_T, merged, recvcounts, rdispls, MPI_DATA_T, comm);

    //*data = merged;

    *data = (data_t*)realloc(*data, new_chunk_size*sizeof(data_t)); // Reallocate the memory to the new size
    // Copy the merged array into the original array
    memcpy(*data, merged, new_chunk_size*sizeof(data_t));
    *chunk_size = new_chunk_size;

    // Step 4: Local sort
    #pragma omp parallel
    {
        #pragma omp single
        par_quicksort(*data, 0, *chunk_size, cmp_ge);
    }

    // Free the memory
    free(pivots);
    free(samples);
    free(selected_pivots);
    free(pivot_positions);
    free(all_chunk_sizes);
    free(all_pivot_positions);
    free(sendcounts);
    free(sdispls);
    free(recvcounts);
    free(rdispls);

}