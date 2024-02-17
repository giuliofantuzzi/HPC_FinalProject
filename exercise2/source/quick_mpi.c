#include "quick_mpi.h"
// ================================================================
//  FUNCTIONS DEFINITION
// ================================================================

#define SWAP(A,B,SIZE) do {int sz = (SIZE); char *a = (A); char *b = (B); \
do { char _temp = *a;*a++ = *b;*b++ = _temp;} while (--sz);} while (0)

int partitioning(data_t* data, int start, int end, compare_t cmp_ge){

    // Pick the median of the [0], [mid] and [end] element as pivot
    int mid = (start + end-1) / 2;
    if (cmp_ge((void*)&data[start], (void*)&data[mid]))
        SWAP((void*)&data[start], (void*)&data[mid], sizeof(data_t));
    if (cmp_ge((void*)&data[mid], (void*)&data[end-1]))
        SWAP((void*)&data[mid], (void*)&data[end-1], sizeof(data_t));
    if (cmp_ge((void*)&data[mid], (void*)&data[start]))
        SWAP((void*)&data[start], (void*)&data[mid], sizeof(data_t));

    // With the operations above now the pivot is at the beginning
    void* pivot = (void*)&data[start];
    // Partition around the pivot
    int pointbreak = start + 1;
    for (int i = start + 1; i < end; ++i){
        if (!cmp_ge((void*)&data[i], pivot)){
            // Move elements less than pivot to the left side
            SWAP((void*)&data[i], (void*)&data[pointbreak], sizeof(data_t));

            ++ pointbreak;
            
        }
    }
    // Put the pivot in the right place
    SWAP((void*)&data[start], (void*)&data[pointbreak - 1], sizeof(data_t));
    // Return the pivot position
    return pointbreak - 1;
}


int show_array(data_t* data, int start, int end){
    for (int i = start; i < end; i++)
        printf("%f ", data[i].data[HOT]);
    printf("\n");
    return 0;
}

int compare(const void* a, const void* b){
    data_t* A = (data_t*)a;
    data_t* B = (data_t*)b;
    double diff = A->data[HOT] - B->data[HOT];

    // return 1 if A > B, 0 if A == B, -1 if A < B
    return ((diff > 0) - (diff < 0));
}

int compare_ge(const void* a, const void* b){
    data_t* A = (data_t*)a;
    data_t* B = (data_t*)b;

    // return 1 if A >= B, 0 if A < B
    return (A->data[HOT] >= B->data[HOT]);
}

void serial_quicksort( data_t *data, int start, int end, compare_t cmp_ge )
{

 #if defined(DEBUG)
 #define CHECK {							\
    if ( verify_partitioning( data, start, end, mid ) ) {		\
      printf( "partitioning is wrong\n");				\
      printf("%4d, %4d (%4d, %g) -> %4d, %4d  +  %4d, %4d\n",		\
	     start, end, mid, data[mid].data[HOT],start, mid, mid+1, end); \
      show_array( data, start, end); }}
 #else
 #define CHECK
 #endif

  int size = end-start;
  if ( size > 2 )
    {
      int mid = partitioning( data, start, end, cmp_ge );

      CHECK;
      
      serial_quicksort( data, start, mid, cmp_ge );    // sort the left half
      serial_quicksort( data, mid+1, end , cmp_ge );   // sort the right half
    }
  else
    {
      if ( (size == 2) && cmp_ge ( (void*)&data[start], (void*)&data[end-1] ) )
	SWAP( (void*)&data[start], (void*)&data[end-1], sizeof(data_t) );
    }
}

#ifdef _OPENMP

void omp_quicksort(data_t *data, int start, int end, compare_t cmp_ge) {
    int size = end - start;
    if (size >  2) {
        int mid = partitioning(data, start, end, cmp_ge);
        // Use OpenMP to parallelize the recursive calls
        CHECK; 
	#pragma omp task
        {
            omp_quicksort(data, start, mid, cmp_ge);
        }
        #pragma omp task
        {
            // Sort the right half
            //printf("RIGHT Recursive call by thread %d\n",omp_get_thread_num());
	        omp_quicksort(data, mid +  1, end, cmp_ge);
         }
        }
    	
    else {
        // Handle small subarrays sequentially
        if ((size ==  2) && cmp_ge((void *)&data[start], (void *)&data[end -  1])) {
            SWAP((void *)&data[start], (void *)&data[end -  1], sizeof(data_t));
        }
    }
}

void omp_quicksort_L1(data_t *data, int start, int end, compare_t cmp_ge) {
    int size = end - start;
    if (size > 2) {
        int mid = partitioning(data, start, end, cmp_ge);
        // Use OpenMP to parallelize the recursive calls
        CHECK;
	if(size>SIZE_L1){
            #pragma omp task
            {
                omp_quicksort(data, start, mid, cmp_ge);
            }
            #pragma omp task
            {
                // Sort the right half
                //printf("RIGHT Recursive call by thread %d\n",omp_get_thread_num());
                omp_quicksort(data, mid +  1, end, cmp_ge);
            }
	}
	else{
	    serial_quicksort(data,start,mid,cmp_ge);
	    serial_quicksort(data,mid+1,end,cmp_ge);
	}
    }
    else {
        // Handle small subarrays sequentially
        if ((size ==  2) && cmp_ge((void *)&data[start], (void *)&data[end -  1])) {
            SWAP((void *)&data[start], (void *)&data[end -  1], sizeof(data_t));
        }
    }
}
#endif


int mpi_partitioning(data_t* data, int start, int end, compare_t cmp_ge, void* pivot){
    // Function that partitions the array into two parts given a pivot
    // and returns the position of the last element of the first part

    // Partition around the pivot
    int pointbreak = start;

    // This can't be done in parallel because of possible data races in the exchanges and pointbreak increment
    // Could be done with a parallel for loop, synchronized by an atomic increment of pointbreak, but it would be slower
    for (int i = start; i < end; ++i){
        if (!cmp_ge((void*)&data[i], pivot)){
            
            // Move elements less than pivot to the left side
            SWAP((void*)&data[i], (void*)&data[pointbreak], sizeof(data_t));

            ++ pointbreak;
            
        }
    }
    // We don't need to Put the pivot in the right place since the mpi pivot might not contain it!
    // SWAP((void*)&data[start], (void*)&data[pointbreak - 1], sizeof(data_t));
    // Return the pivot position
    return pointbreak - 1;
}


void mpi_quicksort (data_t** loc_data, int* chunk_size, MPI_Datatype MPI_DATA_T, MPI_Comm comm,compare_t cmp_ge){
    int rank, num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);
    
    if (num_procs > 1){
        //---------------------------------------------------------------------------------------------------------------------
        // (1) Divide the data into two parts and declare 2 communicators: left and right
        int pivot_rank = (num_procs - 1) / 2;
        MPI_Comm left_comm, right_comm;
        //---------------------------------------------------------------------------------------------------------------------
        // (2) Select global pivot and broadcast it to all processes
        data_t* pivot = (data_t*)malloc(sizeof(data_t));
        data_t* pivots = (data_t*)malloc((num_procs+1)*sizeof(data_t));
        // Generate a random index within each chunk
        srand(time(NULL));
        int random_index = rand() % *chunk_size;
        // Select the random element from the local data
        data_t local_pivot;
        memcpy(&local_pivot, &(*loc_data)[random_index], sizeof(data_t));
        // Gather the randomly selected elements from all processes
        MPI_Gather(&local_pivot, 1, MPI_DATA_T, pivots, 1, MPI_DATA_T, 0, comm);
        if (rank == 0){
	    #if defined(_OPENMP)
		#pragma omp parallel
            	{
                    #pragma omp single
		    omp_quicksort_L1(pivots, 0, num_procs, cmp_ge);
		    //omp_quicksort(pivots, 0, num_procs, cmp_ge);
		    #pragma omp taskwait
		}
	    #else
		serial_quicksort(pivots, 0, num_procs, cmp_ge);
	    #endif
	    memcpy(pivot, &pivots[(num_procs / 2)], sizeof(data_t));
        }
        // Send the pivot to all processes
        MPI_Bcast(pivot, 1, MPI_DATA_T, 0, comm);
        //(void*)pivot;
        //---------------------------------------------------------------------------------------------------------------------
        // (3) Partition the data
        // FIRST WE DO IT FOR THE PIVOT RANK in the case of odd number of processes
        // Idea: ancora prima di ordinare tutte le partizioni io posso partizionare il pivot rank e capire quale partizione ha maggiore
        // Sulla base di ciò setto la variabile flag per capire come dividere destra e sinistra
        // Ma posso anche fare distribuire la partizione più piccola tra tutti gli altri processi
        if((num_procs % 2 != 0)){

            int minor_partition_left; // 1 if the minor partition of the chunk is the left one, 0 if it is the right one
            int minor_partition_size;
            data_t* minor_partition=NULL; 
            data_t* maj_partition=NULL;           

            if((rank == pivot_rank)){
                int pivot_pos = mpi_partitioning(*loc_data, 0, *chunk_size, cmp_ge, pivot);

                if(pivot_pos < (*chunk_size-1) / 2){    
                    minor_partition_left = 1;
                    minor_partition_size = pivot_pos + 1;
                    // minor partition will be the left
                    minor_partition = (data_t*)malloc((minor_partition_size)*sizeof(data_t));
                    //memcpy(minor_partition, *loc_data, minor_partition_size*sizeof(data_t));
                    for (int i = 0; i < minor_partition_size; i++){
                        minor_partition[i] = (*loc_data)[i];
                    }
                    maj_partition = (data_t*)malloc((*chunk_size - minor_partition_size)*sizeof(data_t));
                    //memcpy(maj_partition, &(*loc_data)[pivot_pos+1], (*chunk_size - minor_partition_size)*sizeof(data_t));
                    for (int i = 0; i < (*chunk_size - minor_partition_size); i++){
                        maj_partition[i] = (*loc_data)[pivot_pos+1+i];
                    }
                    free(*loc_data);
                    *loc_data = maj_partition;
                    *chunk_size -= minor_partition_size;
                    //free(maj_partition);
                }
                else{
                    minor_partition_left = 0;
                    minor_partition_size = *chunk_size - (pivot_pos+1);
                    // minor partition will be the right
                    minor_partition = (data_t*)malloc((minor_partition_size)*sizeof(data_t));
                    //memcpy(minor_partition, &(*loc_data)[pivot_pos+1], minor_partition_size*sizeof(data_t));
                    for (int i = 0; i < minor_partition_size; i++){
                        minor_partition[i] = (*loc_data)[pivot_pos+1+i];
                    }
                    maj_partition = (data_t*)malloc(pivot_pos*sizeof(data_t));
                    //memcpy(maj_partition, *loc_data, pivot_pos*sizeof(data_t));
                    for (int i = 0; i < pivot_pos; i++){
                        maj_partition[i] = (*loc_data)[i];
                    }
                    free(*loc_data);
                    *loc_data = maj_partition;
                    *chunk_size = pivot_pos;
                    //free(maj_partition);
                }
            }
            // Wait for the minor partition to be defined
            //MPI_Barrier(comm); //USELESS
            // Broadcast the minor partition size to all processes
            MPI_Bcast(&minor_partition_left, 1, MPI_INT, pivot_rank, comm);
            MPI_Bcast(&minor_partition_size, 1, MPI_INT, pivot_rank, comm);

            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // TO BE DONE
            // Now pivot_rank has to distribute (with scatterv) the minor partition to all processes and then keep just the major partition as its own loc_data
            // Notice that i want to manage the cases in which minor_partition_size is not divisible by the number of the receiving processes
            // and also the case in which the minor_partition_size is less that the number of processes           
            
            // Divide the minor partition into smaller pieces
            int* sendcounts = (int*)malloc(num_procs*sizeof(int));
            int* displs = (int*)malloc(num_procs*sizeof(int));

            int portion_size = minor_partition_size/ (num_procs - 1); // Exclude pivot rank process
            int remainder = minor_partition_size % (num_procs - 1); // Exclude pivot rank process

            int start = 0;
            // DUBBIO SE SIA DA FARE SOLO IN UN PROCESSO QUESTO!
            for (int i = 0; i < num_procs; i++){
                if (i == pivot_rank){
                    sendcounts[i] = 0; // Root process does not receive any data
                } else {
                    sendcounts[i] = portion_size;
                    if (remainder > 0){
                        sendcounts[i] += 1;
                        remainder -= 1;
                    }
                }
                displs[i] = start;
                start += sendcounts[i];
            }
            // Now scatterv the minor partition to all processes
            // data_t* local_minor_partition = (data_t*)malloc(sendcounts[rank] * sizeof(data_t));
            // MPI_Scatterv(&minor_partition[0], sendcounts, displs, MPI_DATA_T, &local_minor_partition[0], sendcounts[rank], MPI_DATA_T, pivot_rank, comm);
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // print if scatterv for successfull for rank
            //printf("rank %d has received from scatterv\n",rank);
            
            // Merge the local_minor_partition with the loc_data
            *loc_data = (data_t*)realloc(*loc_data, (*chunk_size + sendcounts[rank]) * sizeof(data_t));
            MPI_Scatterv(&minor_partition[0], sendcounts, displs, MPI_DATA_T, &(*loc_data)[*chunk_size], sendcounts[rank], MPI_DATA_T, pivot_rank, comm);
            *chunk_size += sendcounts[rank];

            // Append the local_minor_partition to loc_data (QUI INIZIANO I DUBBI)
            //data_t* new_loc_data = (data_t*)malloc((*chunk_size + sendcounts[rank]) * sizeof(data_t))
            //memcpy(new_loc_data, *loc_data, *chunk_size * sizeof(data_t));
            //memcpy(&new_loc_data[*chunk_size], local_minor_partition, sendcounts[rank] * sizeof(data_t));
            // Update loc_data with the combined data
            //free(*loc_data);
            //*loc_data = new_loc_data;
            //*chunk_size += piece_size;
            MPI_Barrier(comm);
            // Clean up memory
            //free(minor_partition);
            // free(local_minor_partition);
            free(displs);
            free(sendcounts);
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // Finally we can define 
            if(minor_partition_left){
                MPI_Comm_split(comm, rank < pivot_rank, rank, &left_comm);
                MPI_Comm_split(comm, rank >= pivot_rank, rank, &right_comm);

            }
            else{
                MPI_Comm_split(comm, rank <= pivot_rank, rank, &left_comm);
                MPI_Comm_split(comm, rank > pivot_rank, rank, &right_comm);
            }
            // // Define the communicators for the recursive
            // Chiamare ricorsione per il pivot rank    
            if (rank == pivot_rank){
                switch (minor_partition_left){
                    case 1:
                        mpi_quicksort(&maj_partition, chunk_size, MPI_DATA_T, right_comm,cmp_ge);
                        *loc_data = maj_partition;
                        break;
                    case 0:
                        mpi_quicksort(&maj_partition, chunk_size, MPI_DATA_T, left_comm,cmp_ge);
                        *loc_data = maj_partition;
                        break;
                }
            }

            // TODO: free the memory of the minor and major partition, reorder the code
        }
        else{ // If number of processes is even
            // Define the communicators for the recursive calls
            MPI_Comm_split(comm, rank <= pivot_rank, rank, &left_comm);
            MPI_Comm_split(comm, rank > pivot_rank, rank, &right_comm);
        }
    
        //---------------------------------------------------------------------------------------------------------------------
        // Now partition all the other chunks and complete all the exchanges
        int pivot_pos = mpi_partitioning(*loc_data, 0, *chunk_size, compare_ge, pivot);
        free(pivot);
        
        //data_t* merged=NULL;
        if (rank < pivot_rank || (num_procs % 2 == 0 && rank == pivot_rank)){ //
            int elements_to_send = *chunk_size - (pivot_pos + 1);
            MPI_Send(&elements_to_send, 1, MPI_INT, rank + pivot_rank + 1, 0, comm);
            int recv_elements;
            MPI_Recv(&recv_elements, 1, MPI_INT, rank + pivot_rank + 1, 0, comm, MPI_STATUS_IGNORE);
            recv_elements = (recv_elements > 0) ? recv_elements : 0;
            int new_chunk_size = *chunk_size - elements_to_send + recv_elements;
            data_t* merged = (data_t*)malloc((new_chunk_size)*sizeof(data_t));
            for (int i = 0; i <= pivot_pos; i++){
                merged[i] = (*loc_data)[i];
            }
            MPI_Send(&(*loc_data)[pivot_pos + 1], elements_to_send, MPI_DATA_T, rank + pivot_rank + 1, 0, comm);
            free(*loc_data);
            MPI_Recv(&merged[pivot_pos + 1], recv_elements, MPI_DATA_T, rank + pivot_rank + 1, 0, comm, MPI_STATUS_IGNORE);
            *chunk_size = new_chunk_size;
            mpi_quicksort(&merged, chunk_size, MPI_DATA_T, left_comm,cmp_ge);
            *loc_data = merged;
        }
        if (rank > pivot_rank){
            int recv_elements;
            MPI_Recv(&recv_elements, 1, MPI_INT, rank - (pivot_rank + 1), 0, comm, MPI_STATUS_IGNORE);
            int elements_to_send = pivot_pos +1;
            MPI_Send(&elements_to_send, 1, MPI_INT, rank - (pivot_rank +1), 0, comm);
            recv_elements = (recv_elements > 0) ? recv_elements : 0;
            int new_chunk_size = *chunk_size - elements_to_send + recv_elements;
            data_t* merged = (data_t*)malloc((new_chunk_size)*sizeof(data_t));
            for (int i = pivot_pos + 1; i < *chunk_size; i++){
                merged[i - (pivot_pos + 1) + recv_elements] = (*loc_data)[i];
            }
            MPI_Recv(&merged[0], recv_elements, MPI_DATA_T, rank - (pivot_rank +1), 0, comm, MPI_STATUS_IGNORE);
            MPI_Send(&(*loc_data)[0], pivot_pos +1, MPI_DATA_T, rank - (pivot_rank +1), 0, comm);
            free(*loc_data);
            *chunk_size = new_chunk_size;
            mpi_quicksort(&merged, chunk_size, MPI_DATA_T, right_comm,cmp_ge);
            *loc_data = merged;
        }
        MPI_Comm_free(&left_comm);
        MPI_Comm_free(&right_comm);
    } else {
        #if defined(_OPENMP)
            #pragma omp parallel
            {
                #pragma omp single
                omp_quicksort_L1(*loc_data, 0, *chunk_size, cmp_ge);
		//omp_quicksort(*loc_data, 0, *chunk_size, cmp_ge);
		#pragma omp taskwait
            }
        #else
            serial_quicksort(*loc_data, 0, *chunk_size, cmp_ge);
	#endif
    }
}

// Verify sorting within chunks
int verify_sorting( data_t *data, int start, int end)
{
    int i = start+1;
    // substituted <= end with <
    while( (i < end) && (data[i].data[HOT] >= data[i-1].data[HOT]) )
        i++;
    return ( i == end );
}

// Verify sorting between threads
int verify_global_sorting( data_t *loc_data, int start, int end, MPI_Datatype MPI_DATA_T, int rank, int num_procs)
{
    // First check that the local array is sorted
    if(verify_sorting( loc_data, start, end)){

        // Then I check that the last element of the local array is less than or equal to the first element of the next process
        if (rank >= 0 && rank < num_procs - 1) {
	    // Send the last element of loc_data to the next process (rank + 1)
	    if (end - start > 0)
	        MPI_Send(&loc_data[end - 1], 1, MPI_DATA_T, rank + 1, 0, MPI_COMM_WORLD);
	    else{
	        // If the array is empty, send a dummy element
	        data_t dummy;
	        // To access the index's element of the dummy array
	        // dummy[index].data[HOT]
	        dummy.data[HOT] = -1;
	        MPI_Send(&dummy, 1, MPI_DATA_T, rank + 1, 0, MPI_COMM_WORLD);
 	    }
        }

        if (rank >0 && rank < num_procs) {
	    data_t prev_last;
	    // Receive the last element from the previous process (rank - 1)
	    MPI_Recv(&prev_last, 1, MPI_DATA_T, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	    // Check if the first element of the current process is lower than the last element of the previous process
	    if (loc_data[0].data[HOT] < prev_last.data[HOT]) {
	        // If not sorted, return 0
	        return 0;
	    }
        }
        // If we arrived here, everything is sorted :)
        return 1;
    }
    else{
        return 0;
    }
}
