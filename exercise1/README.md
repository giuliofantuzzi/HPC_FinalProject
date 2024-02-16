# HPC Final Project: Exercise 1

## Table of contents
- [Project structure](#project-structure)
- [Osu-micro-benchmarks 7.3 installation](#osu-micro-benchmarks-73-installation)
    - [Automatized compilation](#automatized-compilation)
    - [Manual compilation](#manual-compilation)
- [Data gathering](#data-gathering)

## Project structure

```
📂 exercise1/
│ 
├── 📂 barrier/
│   ├── 📂 barrier_bruck/
│   │    └── ⏳ barrier_bruck_getdata.sh
│   │
│   ├── 📂 barrier_default/
│   │    └── ⏳ barrier_default_getdata.sh
│   │
│   ├── 📂 barrier_doublering/
│   │    └── ⏳ barrier_doublering_getdata.sh
│   │
│   ├── 📂 barrier_linear/
│   │    └── ⏳ barrier_linear_getdata.sh
│   │
│   ├── 📂 barrier_recursivedoubling/
│   │    └── ⏳ barrier_recursivedoubling_getdata.sh
│   │
│   ├── 📂 barrier_tree/
│   │    └── ⏳ barrier_tree_getdata.sh
│   │
│   └──  📂 results_barrier/
│        ├──🔎 analysis.Rmd
│        ├── 📊 barrier_bruck.csv
│        ├── 📊 barrier_default.csv
│        ├── 📊 barrier_doublering.csv
│        ├── 📊 barrier_linear.csv
│        ├── 📊 barrier_recursivedoubling.csv
│        └── 📊 barrier_tree.csv
│ 
├── 📂 bcast/
│   ├── 📂 bcast_binarytree/
│   │    └── ⏳ bcast_binarytree_getdata.sh
│   ├── 📂 bcast_chain/
│   │    └── ⏳ bcast_chain_getdata.sh
│   ├── 📂 bcast_default/
│   │    └── ⏳ bcast_default_getdata.sh
│   ├── 📂 bcast_linear/
│   │    └── ⏳ bcast_linear_getdata.sh
│   │
│   └──  📂 results_bcast/
│        ├──🔎 analysis.Rmd
│        ├── 📊 bcast_binarytree.csv
│        ├── 📊 bcast_chain.csv
│        ├── 📊 bcast_default.csv
│        └── 📊 bcast_linear.csv
│
├── 🏗️ compile_OSU-THIN.sh
│
└── 📰 README.md

```

## Osu-micro-benchmarks 7.3 installation

First of all, let's install the zipped file with the *wget* command:

```bash
wget https://mvapich.cse.ohio-state.edu/download/mvapich/osu-micro-benchmarks-7.3.tar.gz
```

For the automatic compilation [click here](#automatized-compilation) \
For the manual compilation [skip here](#manual-compilation)

## Automatized Compilation

I provided you a bash script to compile the OSU library automatically on ORFEO: `compile_OSU-THIN.sh`. Just run it with:

```bash
sbatch compile_OSU-THIN.sh
```

**Note:** such compilation script is specifical for THIN nodes' architecture!

## Manual Compilation

Load openMPI module:

```bash
module load openMPI/4.1.5/gnu
```

Compile the library with:

```bash
./configure CC=/path/to/mpicc CXX=/path/to/mpicxx
make
make install
```

**Note:** the paths above can be found with the *which* command:

```bash
which mpicc  #this returned me /opt/programs/openMPI/4.1.5/bin/mpicc
which mpicxx #this returned me /opt/programs/openMPI/4.1.5/bin/mpicxx 
```

# Data gathering

The entire data gathering process was automated by using some bash scripts, which were then submitted to the cluster using the **SLURM** workload manager, utilizing the *sbatch* command for streamlined execution.

**Important Note:** <u> my personal choice</u> was to mantain separate folders and scripts for the different algorithms. In my opinion it was useful for being more flexible both in the data analysis phase and in the collection phase itself!

**How to get data?** if you want to obtain data for an algorithm of one of the 2 collective operations, just move to the desired folder and run the provided script. 

*Example(linear broadcast)*:
```bash
cd bcast/bcast_linear
sbatch bcast_linear_getdata.sh
```

