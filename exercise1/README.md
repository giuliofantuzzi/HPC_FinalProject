# HPC Final Project: Exercise 1

## Table of contents
- [Project structure](#project-structure)
- [Osu-micro-benchmarks 7.3 installation](#osu-micro-benchmarks-73-installation)
    - [Automatic compilation (suggested)](#automatic-compilation-suggested)
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
│        ├── 🌐 barrier_analysis.html
│        ├── 🔎 barrier_analysis.Rmd
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
│        ├── 🌐 bcast_analysis.html
│        ├── 🔎 bcast_analysis.Rmd
│        ├── 📊 bcast_binarytree.csv
│        ├── 📊 bcast_chain.csv
│        ├── 📊 bcast_default.csv
│        └── 📊 bcast_linear.csv
│
├── 🏗️ compile_OSU-THIN.sh
│
└── 📰 README.md

```

⚠️  **<u>Note</u>** ⚠️ 

When I was gathering data I hadn't already decided the barrier algorithms to analyze, so you'll find csv files and bash scripts regarding more than the requested algorithms. As you will find in my report, I analyzed the following algorithms:
- linear, chain and tree for Broadcast;
- linear, tree and Bruck for Barrier

## Osu-micro-benchmarks 7.3 installation

To perform the benchmarks it is required to download the `osu-micro-benchmarks`.

First of all, let's install the zipped file with the *wget* command:

```bash
wget https://mvapich.cse.ohio-state.edu/download/mvapich/osu-micro-benchmarks-7.3.tar.gz
```

Then, unzip the downloaded folder:

```bash
tar -xzvf osu-micro-benchmarks-7.3.tar.gz
```
The last step is to compile the library:

- For the automatic compilation [click here](#automatic-compilation) 
- For the manual compilation [skip here](#manual-compilation)

## Automatic Compilation (suggested)

I provided you a bash script to compile the OSU library automatically on ORFEO: `compile_OSU-THIN.sh`. Just run it with:

```bash
sbatch compile_OSU-THIN.sh
```

**Note:** such compilation script is specifical for **THIN** nodes' architecture!

## Manual Compilation

Here the instructions to manually compile the library on your pc

Initially, load openMPI module:

```bash
module load openMPI/4.1.5/gnu
```

Compile the library with:

```bash
./configure CC=/path/to/mpicc CXX=/path/to/mpicxx
make
make install
```

where the required paths can be found with:

```bash
which mpicc  #this returned me /opt/programs/openMPI/4.1.5/bin/mpicc
which mpicxx #this returned me /opt/programs/openMPI/4.1.5/bin/mpicxx 
```

# Data gathering

The entire data gathering process was automated by using some bash scripts, which were then submitted to the cluster using the **SLURM** workload manager, utilizing the *sbatch* command for streamlined execution.

**<u>Important Note:</u>** 

**My personal choice** was to mantain separate folders and scripts for the different algorithms. In my opinion it was useful for being more flexible both in the data analysis phase and in the collection phase itself!

**<u>How to get data?</u>** 

If you want to obtain data for an algorithm of one of the 2 collective operations, just move to the desired folder and run the provided script. 

*Example(linear broadcast)*:
```bash
cd bcast/bcast_linear
sbatch bcast_linear_getdata.sh
```

