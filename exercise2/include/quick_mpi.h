// ================================================================
//  LIBRARIES 
// ================================================================
#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>
#include <mpi.h>

// ================================================================
//  MACROS and DATATYPES
// ================================================================


// #if defined(_OPENMP)

// // measure the wall-clock time
// #define CPU_TIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec + \
//                   (double)ts.tv_nsec * 1e-9)

// // measure the cpu thread time
// #define CPU_TIME_th (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), (double)myts.tv_sec +     \
//                      (double)myts.tv_nsec * 1e-9)

// #else

// // measure ther cpu process time
// #define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec + \
//                   (double)ts.tv_nsec * 1e-9)
// #endif

#if !defined(SIZE_L1)
#define SIZE_L1 65536
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

// Define the default amount of data
#if (!defined(DEBUG) || defined(_OPENMP))
#define N_dflt    100000
#else
#define N_dflt    10000
#endif

// Define the data_t struct
typedef struct {
    double data[DATA_SIZE];
} data_t;

// Define macros for min and max between data_t objects
#define MIN(a,b) ( (a)->data[HOT] < (b)->data[HOT]? (a) : (b));
#define MAX(a,b) ( (a)->data[HOT] > (b)->data[HOT]? (a) : (b));
#define SWAP(A,B,SIZE) do {int sz = (SIZE); char *a = (A); char *b = (B); \
do { char _temp = *a;*a++ = *b;*b++ = _temp;} while (--sz);} while (0)

// ================================================================
//  FUNCTION PROTOTYPES
// ================================================================

// Define compare function that will be used by qsort
typedef int compare_t(const void *, const void *);

// Define verifying function type, used to test results
typedef int verify_t(data_t *, int, int);

// ================================================================
//  FUNCTIONS DECLARATION
// ================================================================
      
compare_t compare_ge;   
verify_t verify_partitioning;
verify_t verify_sorting;
verify_t show_array;
int verify_global_sorting(data_t*, int, int, MPI_Datatype, int, int);
int partitioning(data_t*, int, int, compare_t);
int mpi_partitioning(data_t*, int, int, compare_t, void*);

// Serial Quicksort
void serial_quicksort( data_t *, int, int, compare_t ); 
// OpenMP Quicksort
void omp_quicksort(data_t*, int, int, compare_t);
// OpenMP Quicksort with L1 cache optimization
void omp_quicksort_L1(data_t *, int, int, compare_t);
// MPI Quicksort
void mpi_quicksort(data_t**, int*, MPI_Datatype, MPI_Comm,compare_t);