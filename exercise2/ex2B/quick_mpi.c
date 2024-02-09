// ================================================================
//  LIBRARIES AND GLOBAL DEFINITIONS
// ================================================================
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

#if !defined(DATA_SIZE)
#define DATA_SIZE 8
#endif
#define HOT       0

#define SIZE 100

typedef struct {
    double data[SIZE];
} data_t;

// Define macros for min and max between data_t objects
#define MIN(a,b) ( (a)->data[HOT] < (b)->data[HOT]? (a) : (b));
#define MAX(a,b) ( (a)->data[HOT] > (b)->data[HOT]? (a) : (b));

#if defined(_OPENMP)

// measure the wall-clock time
#define CPU_TIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec + \
                  (double)ts.tv_nsec * 1e-9)

// measure the cpu thread time
#define CPU_TIME_th (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), (double)myts.tv_sec +     \
                     (double)myts.tv_nsec * 1e-9)

#else

// measure ther cpu process time
#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec + \
                  (double)ts.tv_nsec * 1e-9)
#endif

// ================================================================
//  FUNCTION PROTOTYPES
// ================================================================

// Define compare function that will be used by qsort
typedef int compare_t(const void *, const void *);

// Define verifying function type, used to test results
typedef int verify_t(data_t *, int, int, int);

// Declare the functions
compare_t compare;      // compare function
compare_t compare_ge;   // compare function for "greater or equal"
verify_t verify_partitioning;
verify_t verify_sorting;
verify_t show_array;
//extern inline int partitioning( data_t *, int, int, compare_t );
int partition(data_t*, int, int, compare_t);
int mpi_partition(data_t*, int, int, compare_t, void*);
void mpi_quicksort1(data_t**, int*, MPI_Datatype, MPI_Comm);
// void mpi_quicksort(data_t*, int*, int, int, int, MPI_Datatype, compare_t);
int verify_global_sorting(data_t*, int, int, MPI_Datatype, int, int, int);
// SERIAL QUICKSORT
void serial_quicksort( data_t *, int, int, compare_t ); 
// OMP QUICKSORT
void omp_quicksort(data_t *data, int start, int end, compare_t cmp_ge);


// ================================================================
//  MAIN PROGRAM
// ================================================================
int main(int argc, char** argv){
    //----------------------------------------------------------------------------------------------
    // (1) INITIALIZATION
    int N = SIZE;
    //int nthreads=1;
    {
        int a = 0;
        if ( argc > ++a ) N = atoi(*(argv+a));
    }

    char* env_var = getenv("OMP_NUM_THREADS");
    if (env_var != NULL) {
        int nthreads = atoi(env_var);
    } else {
        printf("OMP_NUM_THREADS environment variable not set :(\n");
    }

    int n_processes, rank;
    int mpi_err = MPI_Init(&argc, &argv);

    if (mpi_err != MPI_SUCCESS) {
        printf("Error initializing MPI program :(\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype MPI_DATA_T;
    MPI_Type_contiguous(sizeof(data_t), MPI_BYTE, &MPI_DATA_T);
    MPI_Type_commit(&MPI_DATA_T);

    // ----------------------------------------------------------------------------------------------
    //  (2) PROCESS CHUNK OF DATA

    int chunk_size = (rank < N % n_processes) ? N / n_processes + 1 : N / n_processes;
    data_t *data = (data_t*)malloc(chunk_size*sizeof(data_t));
    // set a seed depending on the process rank
    long int seed = rank;
    #if defined(_OPENMP)
    #pragma omp parallel
    {
        int me             = omp_get_thread_num();
        short int seeds[3] = {seed-me, seed+me, seed+me*2};

    #pragma omp for
        for ( int i = 0; i < N; i++ )
        data[i].data[HOT] = erand48( seeds );
    }
    #else
    {
        //seed = time(NULL);
        srand48(seed);
        
        PRINTF("ssed is % ld\n", seed);
        
        for ( int i = 0; i < N; i++ )
        data[i].data[HOT] = drand48();
    }    
    #endif

    // ----------------------------------------------------------------------------------------------
    // (3) SORT THE DATA
    // Wait all processes to finish generating the data
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start, t_end;
    t_start= MPI_Wtime();

    mpi_quicksort1(&data, &chunk_size, MPI_DATA_T, MPI_COMM_WORLD);
    
    t_end = MPI_Wtime();
    // ----------------------------------------------------------------------------------------------


    MPI_Barrier(MPI_COMM_WORLD);

    // ---------------------------------------------
    // Show the sorted array
    for (int i = 0; i < n_processes; i++){
        if (rank == i){
            printf("Process %d has sorted:\n", rank);
            show_array(data, 0, chunk_size, 0);
        }
        //MPI_Barrier(MPI_COMM_WORLD);
    }

    double time = t_end - t_start;
    MPI_Barrier(MPI_COMM_WORLD);
    // ---------------------------------------------
    // Verify the results
    int test = verify_global_sorting(data, 0, chunk_size, MPI_DATA_T, rank, n_processes, 0);

    // int global_test = 0;
    // MPI_Reduce(&test, &global_test, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    //if (rank == 0){
        //if (global_test == n_processes)
    //    printf("Test result is %d\n", global_test);
    //    printf("Time elapsed is %f\n", time);
        // else
        // printf("Test failed\n");
    //}

    // printf("Rank %d has loc_data of size %d\n", rank, chunk_size);
    //free(data);
    free(data);
    // free(sorted);
    // free(sorted);
    // free(merged);
    MPI_Type_free(&MPI_DATA_T);
    //printf("Data freed\n");
    //MPI_Finalize();
    int finalize_retcode = MPI_Finalize();
    fprintf(stderr, "Process, return_code\n");
    fprintf(stderr, "%i, %i\n", rank, finalize_retcode);
    // free(try);
    // free(chunk_sizes);

    return 0;
}




#define SWAP(A,B,SIZE) do {int sz = (SIZE); char *a = (A); char *b = (B); \
do { char _temp = *a;*a++ = *b;*b++ = _temp;} while (--sz);} while (0)

int partition(data_t* data, int start, int end, compare_t cmp_ge){

    // Pick the median of the [0], [mid] and [end] element as pivot
    int mid = (start + end-1) / 2;
    if (cmp_ge((void*)&data[start], (void*)&data[mid]))
        SWAP((void*)&data[start], (void*)&data[mid], sizeof(data_t));
    if (cmp_ge((void*)&data[mid], (void*)&data[end-1]))
        SWAP((void*)&data[mid], (void*)&data[end-1], sizeof(data_t));
    if (cmp_ge((void*)&data[mid], (void*)&data[start]))
        SWAP((void*)&data[start], (void*)&data[mid], sizeof(data_t));

    // Pick the first element as pivot
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


int show_array(data_t* data, int start, int end, int not_used){
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
      show_array( data, start, end, 0 ); }}
 #else
 #define CHECK
 #endif

  int size = end-start;
  if ( size > 2 )
    {
      int mid = partition( data, start, end, cmp_ge );

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
        int mid = partition(data, start, end, cmp_ge);

        // Use OpenMP to parallelize the recursive calls
        CHECK; 
	#pragma omp task
        {
          // Sort the left half
	  //printf("LEFT Recursive call by thread %d\n",omp_get_thread_num());
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

#endif


int mpi_partition(data_t* data, int start, int end, compare_t cmp_ge, void* pivot){
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

    // // Put the pivot in the right place
    // SWAP((void*)&data[start], (void*)&data[pointbreak - 1], sizeof(data_t));

    // Return the pivot position
    return pointbreak - 1;
}


void mpi_quicksort1 (data_t** loc_data, int* chunk_size, MPI_Datatype MPI_DATA_T, MPI_Comm comm){
    int rank, num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);
    
    if (num_procs > 1){
        int pivot_rank = (num_procs - 1) / 2;
        //printf("Pivot rank is %d\n", pivot_rank);
        data_t* pivot = (data_t*)malloc(sizeof(data_t));
        data_t* pivots = (data_t*)malloc((num_procs+1)*sizeof(data_t));
        // MPI_Gather(&(*loc_data)[(*chunk_size -1)/2], 1, MPI_DATA_T, pivots, 1, MPI_DATA_T, 0, comm);
        // Generate a random index within each chunk
        srand(time(NULL));
        int random_index = rand() % *chunk_size;

        // Select the random element from the local data
        data_t local_pivot;
        memcpy(&local_pivot, &(*loc_data)[random_index], sizeof(data_t));

        // Gather the randomly selected elements from all processes
        MPI_Gather(&local_pivot, 1, MPI_DATA_T, pivots, 1, MPI_DATA_T, 0, comm);
        if (rank == 0){
            omp_quicksort(pivots, 0, num_procs, compare_ge);
            memcpy(pivot, &pivots[(num_procs / 2)], sizeof(data_t));
        }
        MPI_Bcast(pivot, 1, MPI_DATA_T, 0, comm);
        (void*)pivot;
        int pivot_pos = mpi_partition(*loc_data, 0, *chunk_size, compare_ge, pivot);
        free(pivot);

        MPI_Comm left_comm, right_comm;
        MPI_Comm_split(comm, rank <= pivot_rank, rank, &left_comm);

        // To manage the case where the number of processes is odd
        (num_procs % 2 == 0) ? MPI_Comm_split(comm, rank > pivot_rank, rank, &right_comm) : MPI_Comm_split(comm, rank >= pivot_rank, rank, &right_comm);

        // To avoid deadlock, the pivot process will call the function recursively
        if (num_procs % 2 != 0 && rank == pivot_rank){
            mpi_quicksort1(loc_data, chunk_size, MPI_DATA_T, left_comm);
            mpi_quicksort1(loc_data, chunk_size, MPI_DATA_T, right_comm);
        }

        if (rank < pivot_rank || (num_procs % 2 == 0 && rank == pivot_rank)){
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
            mpi_quicksort1(&merged, chunk_size, MPI_DATA_T, left_comm);
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
            mpi_quicksort1(&merged, chunk_size, MPI_DATA_T, right_comm);
            *loc_data = merged;
        }
        MPI_Comm_free(&left_comm);
        MPI_Comm_free(&right_comm);
    } else {
        #ifdef _OPENMP
            #pragma omp parallel
            {
                #pragma omp single
                omp_quicksort(*loc_data, 0, *chunk_size, compare_ge);
            }
        #else
            serial_quicksort(*loc_data, 0, *chunk_size, compare_ge);
	#endif
    }
}

// Single chunk version
int verify_sorting( data_t *data, int start, int end, int not_used )
{
    int i = start;
    while( (i <= end) && (data[i].data[HOT] >= data[i-1].data[HOT]) )
        i++;
    return ( i == end );
}

// Global version
int verify_global_sorting( data_t *loc_data, int start, int end, MPI_Datatype MPI_DATA_T, int rank, int num_procs, int not_used )
{
    // First I check that the local array is sorted
    verify_sorting( loc_data, start, end, not_used );

    // Then I check that the last element of the local array is less than or equal to the first element of the next process
    if (rank >= 0 && rank < num_procs - 1) {
        // Send the last element of loc_data to the next process (rank + 1)
        if (end - start > 0)
            MPI_Send(&loc_data[end - start - 1], 1, MPI_DATA_T, rank + 1, 0, MPI_COMM_WORLD);
        else{
            // If the array is empty, send a dummy element
            data_t dummy;
            // To access the index's element of the dummy array
            // dummy[index].data[HOT]
            dummy.data[HOT] = -1;
            MPI_Send(&dummy, 1, MPI_DATA_T, rank + 1, 0, MPI_COMM_WORLD);
        }
    }

    if (rank >0 && rank <= num_procs - 1) {
        data_t prev_last;
        // Receive the last element from the previous process (rank - 1)
        MPI_Recv(&prev_last, 1, MPI_DATA_T, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Check if the first element of the current process is greater than or equal to the last element of the previous process
        if (loc_data[0].data[HOT] > prev_last.data[HOT]) {
            // If not sorted, return 0
            return 0;
        }
    }

    // If everything is fine, return 1 to indicate global sorting
    return 1;

    // A LIVELLO DI SINGOLO PROCESSO FUNZIONA, MA BISOGNEREBBE TROVARE IL MODO DI ACCUMULARE I CODICI DI USCITA SU UN PROCESSO
    // (ad esempio il master) E POI FARE UNA VERIFICA GLOBALE (IDEA: USARE UNA MPI_REDUCE E VEDERE SE LA SOMMA DEI CODICI DI USCITA 
    // E' UGUALE AL NUMERO DI PROCESSI).

    // OGNUNO AVRA IL SUO CODICE DI USCITA, CHE SARA' 1 SE TUTTO E' ANDATO BENE, 0 ALTRIMENTI. POI RANK0 FARA' UNA REDUCE SUI CODICI
    // DI USCITA E SE LA SOMMA E' UGUALE AL NUMERO DI PROCESSI, ALLORA TUTTO E' ANDATO BENE.
}


