#include "quick_mpi.h"

int main(int argc, char** argv){
    //.................................................................................................
    // (1) INITIALIZATION 
    //.................................................................................................

    // Size of the "full" array
    int N = N_dflt;
    if ( argc > 1 ) N = atoi(*(argv+1));
    
    // Set the number of threads
    char* env_var = getenv("OMP_NUM_THREADS");
    int n_threads;
    if (env_var != NULL) {
        omp_set_num_threads(atoi(env_var));
        n_threads=atoi(env_var);
    } else {
        // If the environment variable is not set, use only one thread
        omp_set_num_threads(1);
        n_threads=1;
    }

    // Initialize MPI
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

    //.................................................................................................
    //  (2) GENERATE CHUNK OF DATA FOR THE PROCESS
    //.................................................................................................

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
        for ( int i = 0; i < chunk_size; i++ )
        data[i].data[HOT] = erand48( seeds );
    }
    #else
    {
        //seed = time(NULL);
        srand48(seed);
        
        printf("ssed is % ld\n", seed);
        
        for ( int i = 0; i < chunk_size; i++ )
        data[i].data[HOT] = drand48();
    }    
    #endif
    
    MPI_Barrier(MPI_COMM_WORLD);

    //.................................................................................................
    // (3) SORT THE DATA AND MEASURE TIME
    //.................................................................................................

    // Wait all processes to finish generating the data
    double t_start, t_end;
    MPI_Barrier(MPI_COMM_WORLD);
    t_start= MPI_Wtime();
    mpi_quicksort(&data, &chunk_size, MPI_DATA_T, MPI_COMM_WORLD,compare_ge);
    MPI_Barrier(MPI_COMM_WORLD); // Wait all processes to finish 
    t_end = MPI_Wtime();
    double time = t_end - t_start;
    
    //.................................................................................................
    // (4) VERIFY THE SORTING
    //.................................................................................................

    int chunk_check = verify_global_sorting(data, 0, chunk_size, MPI_DATA_T, rank, n_processes);
    MPI_Barrier(MPI_COMM_WORLD);
    int global_check = 0;
    MPI_Reduce(&chunk_check, &global_check, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0){
        printf("\n");
	if (global_check == n_processes){
	    printf("%d,%d,%f",n_processes,n_threads,time);
        } else {
            printf("The array has not been sorted correctly :(\n");
        }
    }
    //.................................................................................................
    // (4) FINALIZATION 
    //.................................................................................................
    MPI_Barrier(MPI_COMM_WORLD);
    free(data);
    MPI_Type_free(&MPI_DATA_T);
    MPI_Finalize();
    //int finalize_retcode = MPI_Finalize();
    //fprintf(stderr, "Process, return_code\n");
    //fprintf(stderr, "%i, %i\n", rank, finalize_retcode);
    return 0;
}