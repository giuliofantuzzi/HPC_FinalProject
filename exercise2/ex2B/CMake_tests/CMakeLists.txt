# General settings
cmake_minimum_required(VERSION  3.5)
project(HPC_Exam_ex2B)

# Local variables
set(INCLUDE_DIRS "include/")
set(SRC_DIRS "source/")
set(EXE_DIRS "exe/")

set(INCLUDE_FILES "quick_mpi.h")
set(SRC_FILES "quick_mpi.c")
set(MAIN_FILE "main.c")
set(EXE_FILE "main.x")

# Find MPI and OpenMP
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

find_package(OpenMP REQUIRED)
include_directories(${OpenMP_INCLUDE_PATH})

# Compilation
add_executable(${EXE_FILE} ${SRC_DIRS}${SRC_FILES} ${MAIN_FILE})

# Set compile options for OpenMP
if(OPENMP_FOUND)
    target_compile_options(${EXE_FILE} PRIVATE ${OpenMP_C_FLAGS})
endif()

# Link libraries
target_include_directories(${EXE_FILE} PUBLIC ${INCLUDE_DIRS} ${MPI_INCLUDE_PATH} ${OpenMP_INCLUDE_PATH})
target_link_libraries(${EXE_FILE} ${MPI_LIBRARIES} ${OpenMP_LIBRARIES} gomp)

# Set runtime output directory
set_target_properties(${EXE_FILE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../${EXE_DIRS}")

##------------------------------------------------------------
## General settings
#cmake_minimum_required(VERSION 3.5)
#project(HPC_Exam_ex2B)
##----------------------------------------------------------------------------------------------------
## Local variables
#set(INCLUDE_DIRS "include/")
#set(SRC_DIRS "source/")
#set(EXE_DIRS "exe/")
#
#set(INCLUDE_FILES "quick_mpi.h")
#set(SRC_FILES "quick_mpi.c")
#set(MAIN_FILE "main.c")
#set(EXE_FILE "main.x")
##----------------------------------------------------------------------------------------------------
## Find MPI and OpenMP
#find_package(MPI REQUIRED)
#include_directories(${MPI_INCLUDE_PATH})
## and in standard compilation i use -fopenmp, how do i do that?
#find_package(OpenMP REQUIRED)
#include_directories(${OpenMP_INCLUDE_PATH})
##----------------------------------------------------------------------------------------------------
## Compilation
#add_executable(${EXE_FILE} ${SRC_DIRS}${SRC_FILES} ${MAIN_FILE})
#target_include_directories(${EXE_FILE} PUBLIC ${INCLUDE_DIRS} ${MPI_INCLUDE_PATH} ${OpenMP_INCLUDE_PATH})
#if(OPENMP_FOUND)
#    target_compile_options(${EXE_FILE} PRIVATE ${OpenMP_C_FLAGS})
#endif()
#
#target_link_libraries(${EXE_FILE} ${MPI_LIBRARIES} ${OpenMP_LIBRARIES})
#set_target_properties(${EXE_FILE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../${EXE_DIRS}")
##----------------------------------------------------------------------------------------------------
