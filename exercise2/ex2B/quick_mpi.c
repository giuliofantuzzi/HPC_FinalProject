// ===============================================================================================
//  PROFESSOR STUFF
// ===============================================================================================
#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
#include <time.h>

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


#if defined(DEBUG)
#define VERBOSE
#endif

#if defined(VERBOSE)
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#if !defined(DATA_SIZE)
#define DATA_SIZE 8
#endif
#define HOT       0

// let's define the default amount of data
#if (!defined(DEBUG) || defined(_OPENMP))
#define N_dflt    100000
#else
#define N_dflt    10000
#endif


// let's define the data_t type
typedef struct
{
  double data[DATA_SIZE];
} data_t;

// let's defined convenient macros for max and min between two data_t objects
#define MAX( a, b ) ( (a)->data[HOT] >(b)->data[HOT]? (a) : (b) );
#define MIN( a, b ) ( (a)->data[HOT] <(b)->data[HOT]? (a) : (b) );


// let'ìs define the compare funciton that will be used by the sorting routine
typedef int (compare_t)(const void*, const void*);

// let's define the verifying function type, used to test the results
typedef int (verify_t)(data_t *, int, int, int);

// declare the funcitons
extern inline compare_t compare;        // the compare function
extern inline compare_t compare_ge;     // the compare for "greater or equal"
verify_t  verify_partitioning;          // verification functions
verify_t  verify_sorting;
verify_t  show_array;
// declare the partitioning function
extern inline int partitioning( data_t *, int, int, compare_t );
// declare the sorting function
// ===============================================================================================
// SERIAL QUICKSORT
void serial_quicksort( data_t *, int, int, compare_t ); 
// PARALLEL QUICKSORT
void omp_quicksort(data_t *data, int start, int end, compare_t cmp_ge);
// Function to merge two data_t structs
data_t* merge(data_t* v1, int size1, data_t* v2, int size2,compare_t cmp_ge);

// ===============================================================================================
int main(int argc, char** argv) {
    //-----------------------------------------------------------------------------
    // (1) INITIALIZE MPI
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Datatype MPI_DATA_T;
    MPI_Type_contiguous(sizeof(data_t), MPI_BYTE, &MPI_DATA_T);
    MPI_Type_commit(&MPI_DATA_T);
    //-----------------------------------------------------------------------------
    // (2) GENERATES PROCESS CHUNK OF DATA
    int N=N_dflt; 
    /* check command-line arguments */
    {
        int a = 0;
        if ( argc > ++a ) N = atoi(*(argv+a));
    }
    int local_data_size = N / n_processes; // Assuming N is the total size of the data
    data_t* local_data = malloc(local_data_size * sizeof(data_t));
    // Generate random data locally
    srand(time(NULL) + rank);
    for (int i =  0; i < local_data_size; i++) {
        local_data[i] = rand() %  100; // Random numbers between  0 and  99
    }
    //-----------------------------------------------------------------------------
    // (3) START TIMING
    struct timespec ts;
    double tstart, tend;
    tstart = CPU_TIME;
    //-----------------------------------------------------------------------------
    // (4) SORTS LOCAL DATA
    #if defined(_OPENMP)
        #pragma omp parallel
        {
            #pragma omp master
            {
                omp_quicksort(local_data,  0, N, compare_ge);
            }
        }
    #else
        serial_quicksort(local_data, 0, N, compare_ge );
    #endif
    //-----------------------------------------------------------------------------
    // (5) GLOBAL MERGING
    // In process 0 create the final array
    if(rank==0){
        data_t* data = (data_t*)malloc(N*sizeof(data_t));
    }
    // Return the sorted data to the master process by recursively merging two sorted arrays
    for (int step = 1; step < n_processes; step = 2 * step) {
        if (rank % (2 * step) != 0) {
            MPI_Send(local_data, local_data_size, MPI_DATA_T, rank - step, 0, MPI_COMM_WORLD);
            break;
        }
        if (rank + step < n_processes) {
            int received_data_size = (N >= local_data_size * (rank + 2 * step)) ? local_data_size * step : (N - local_data_size * (rank + step));
            data_t* received_data = (data_t*)malloc(received_data_size*sizeof(data_t));

            MPI_Recv(received_data, received_data_size, MPI_DATA_T, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Merge the received array with own array
            data = merge(local_data, local_data_size, received_data, received_data_size, compare_ge);

            // Free the memory
            free(local_data);
            free(received_data);

            // Update the chunk pointer
            local_data = data;
            local_data_size += received_data_size;
        }
    }
    // Return the sorted array to the data pointer
    data = local_data;
    // Release chunk
    free(local_data);
    //-----------------------------------------------------------------------------
    // (6) END TIMING
    tend = CPU_TIME;
    //-----------------------------------------------------------------------------
    // (7) MPI FINALIZE and print results
    MPI_Finalize();
    if(rank==0){
        if ( verify_sorting( data, 0, N, 0) ){
            printf("MPI Execution with %d processes in a total time of %g sec\n",n_processes,tend-tstart);
            //printf("Sorted data:\n");
            //show_array(data, 0, N, 0);
        }
        else{
            printf("the array is not sorted correctly\n");
        }
    }
    free(data);
}

// ===============================================================================================
// FUNCTIONS DEFINITION
#define SWAP(A,B,SIZE) do {int sz = (SIZE); char *a = (A); char *b = (B); \
    do { char _temp = *a;*a++ = *b;*b++ = _temp;} while (--sz);} while (0)

inline int partitioning( data_t *data, int start, int end, compare_t cmp_ge )
{
  //--------------------------------------------------------------------------
  // (1) pick up the meadian of [0], [mid] and [end] as pivot
  int mid = start + (end - start) / 2;
  // Swap the middle element with the last element to use it as the pivot
  SWAP((void*)&data[mid], (void*)&data[end - 1], sizeof(data_t));
  // Use the last element as the pivot
  void *pivot = (void*)&data[end - 1];
  // Partition around the pivot element
  int pointbreak = end - 2; // Adjust the starting point for the loop
  //--------------------------------------------------------------------------
  // (2) pick up the last element as pivot
  // //
  // --end;  
  // void *pivot = (void*)&data[end];
  // // partition around the pivot element
  //int pointbreak = end-1;
  //--------------------------------------------------------------------------
  for ( int i = start; i <= pointbreak; i++ )
    if( cmp_ge( (void*)&data[i], pivot ) )
    {
	    while( (pointbreak > i) && cmp_ge( (void*)&data[pointbreak], pivot ) ) pointbreak--;
	    if (pointbreak > i ) 
	      SWAP( (void*)&data[i], (void*)&data[pointbreak--], sizeof(data_t) );
    }  
    pointbreak += !cmp_ge( (void*)&data[pointbreak], pivot ) ;
    SWAP( (void*)&data[pointbreak], pivot, sizeof(data_t) ); 
  return pointbreak;
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



int verify_sorting( data_t *data, int start, int end, int not_used )
{
  int i = start;
  while( (++i < end) && (data[i].data[HOT] >= data[i-1].data[HOT]) );
  return ( i == end );
}

int verify_partitioning( data_t *data, int start, int end, int mid )
{
  int failure = 0;
  int fail = 0;
  
  for( int i = start; i < mid; i++ )
    if ( compare( (void*)&data[i], (void*)&data[mid] ) >= 0 )
      fail++;

  failure += fail;
  if ( fail )
    { 
      printf("failure in first half\n");
      fail = 0;
    }

  for( int i = mid+1; i < end; i++ )
    if ( compare( (void*)&data[i], (void*)&data[mid] ) < 0 )
      fail++;

  failure += fail;
  if ( fail )
    printf("failure in second half\n");

  return failure;
}


int show_array( data_t *data, int start, int end, int not_used )
{
  for ( int i = start; i < end; i++ )
    printf( "%f ", data[i].data[HOT] );
  printf("\n");
  return 0;
}


inline int compare( const void *A, const void *B )
{
  data_t *a = (data_t*)A;
  data_t *b = (data_t*)B;

  double diff = a->data[HOT] - b->data[HOT];
  return ( (diff > 0) - (diff < 0) );
}

inline int compare_ge( const void *A, const void *B )
{
  data_t *a = (data_t*)A;
  data_t *b = (data_t*)B;

  return (a->data[HOT] >= b->data[HOT]);
}




data_t* merge(data_t* v1, int size1, data_t* v2, int size2,compare_t cmp_ge) {

    data_t* merged = (data_t*)malloc((size1 + size2)*sizeof(data_t));
    int i = 0, j = 0, k = 0;
    while (i < size1 && j < size2) {
        if (cmp_ge((void*)&v2[j], (void*)&v1[i])) {
            merged[k++] = v1[i++];
        } else {
            merged[k++] = v2[j++];
        }
    }
    while (i < size1) {
        merged[k++] = v1[i++];
    }
    while (j < size2) {
        merged[k++] = v2[j++];
    }
    return merged;
}
