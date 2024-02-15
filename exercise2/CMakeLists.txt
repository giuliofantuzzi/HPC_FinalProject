#----------------------------------------------------------------------------------------------------------
# General settings
cmake_minimum_required(VERSION  3.5)
project(HPC-Exam-Ex2B
        DESCRIPTION "High Performance Computing - Exam - Exercise 2B"
        VERSION 1.0)
set(CMAKE_C_FLAGS "-O3 -march=native")
#----------------------------------------------------------------------------------------------------------
# Local variables
# Directories
set(INCLUDE_DIRS "include/")
set(SRC_DIRS "source/")
set(MAIN_DIRS "apps/")
set(EXE_DIRS "apps/")
# Scripts
set(INCLUDE_FILES "quick_mpi.h")
set(SRC_FILES "quick_mpi.c")
set(MAIN_FILE "main.c")
set(EXE_FILE "main.x")
#----------------------------------------------------------------------------------------------------------
# Find required packages
# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
# OpenMP
find_package(OpenMP REQUIRED)
include_directories(${OpenMP_INCLUDE_PATH})
#----------------------------------------------------------------------------------------------------------

# Compilation
add_executable(${EXE_FILE} ${SRC_DIRS}${SRC_FILES} ${MAIN_DIRS}${MAIN_FILE})
# Set compile options for OpenMP
if(OPENMP_FOUND)
    target_compile_options(${EXE_FILE} PRIVATE ${OpenMP_C_FLAGS})
endif()
# Link libraries
target_include_directories(${EXE_FILE} PUBLIC ${INCLUDE_DIRS} ${MPI_INCLUDE_PATH} ${OpenMP_INCLUDE_PATH})
target_link_libraries(${EXE_FILE} ${MPI_LIBRARIES} ${OpenMP_LIBRARIES} gomp)
# Set runtime output directory
set_target_properties(${EXE_FILE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../${EXE_DIRS}")
#----------------------------------------------------------------------------------------------------------


