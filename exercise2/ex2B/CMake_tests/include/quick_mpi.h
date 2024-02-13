// ================================================================
//  LIBRARIES AND GLOBAL SETTINGS
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
//  FUNCTIONS DECLARATION
// ================================================================

typedef int compare_t(const void *, const void *);
typedef int verify_t(data_t *, int, int, int);
compare_t compare;      
compare_t compare_ge;   
verify_t verify_partitioning;
verify_t verify_sorting;
verify_t show_array;
int partitioning(data_t*, int, int, compare_t);
int mpi_partitioning(data_t*, int, int, compare_t, void*);
void mpi_quicksort(data_t**, int*, MPI_Datatype, MPI_Comm);
int verify_global_sorting(data_t*, int, int, MPI_Datatype, int, int, int);

// SERIAL QUICKSORT
void serial_quicksort( data_t *, int, int, compare_t ); 
// OMP QUICKSORT
void omp_quicksort(data_t *data, int start, int end, compare_t cmp_ge);
