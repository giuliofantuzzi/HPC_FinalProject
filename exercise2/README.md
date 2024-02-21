# HPC Final Project: Exercise 2B

## Table of contents
- [Project structure](#project-structure)
- [How to build](#how-to-build)
- [How to run](#how-to-run)
- [Get timings](#get-timings)

## Project structure

```
📂 ex2B/
│ 
├── 📂 apps/
│   └── 📄 main.c
│
├── 📂 include/
│   └── 📄 hybrid_qsort.h
│
├── 📂 source/
│   └── 📄 hybrid_qsort.c
│
├── 📂 scalability/
│   ├── 🌐 scalability_analysis.html
│   └── 🔎 scalability_analysis.Rmd
│
├── 📂 timings/
│   ├── ⏳ getdata_omp.sh
│   ├── ⏳ getdata_strong.sh
│   ├── ⏳ getdata_weak.sh
│   ├── 📊 omp_L1_640M.csv
│   ├── 📊 omp_standard_640M.csv
│   ├── 📊 StrongScalability_160M_1-64.csv
│   ├── 📊 StrongScalability_160M_65-128.csv
│   ├── 📊 WeakScalability_160M_1-64.csv
│   └── 📊 WeakScalability_160M_65-128.csv
│
├── 🏗️ build.sh
│
├── 📝 CMakeLists.txt
│
└── 📰 README.md
```
## How to build
 
I decided to provide a `CMakeLists.txt` to facilitate the compilation of the C scripts.

To build on ORFEO, you simply need to run the command:

```bash
sbatch build.sh
```

If you want to build the files in your pc, do instead:

```bash
mkdir build && cd build
cmake ..
make
```

**Note**: this second way assumes that you've already installed `MPI` and `openMP` libraries on your pc!

If the build phase was successfull (hopefully it should be 😅), you will find the executable [`apps/main.x`](apps/main.x) inside the folder [`apps/`](apps/)

## How to run

Once built the executable, the first thing to do is setting the desired openMP options (since the code is hybrid, you should be interested in doing that!). For example, to specify the number of threads to use:

```bash
export OMP_NUM_THREADS=...
```

When completed this preliminary part, the code can be run with `mpirun` with the following syntax:

```bash
mpirun -np P --map-by socket /path/to/main.x N
```
where:

- **P** is the number of MPI processes to use
- **N** is the overall size of the array to sort (if not specified, the default will be N=100000)
- **--map-by socket** is the optimal allocation type in relation to the algorithm's structure (but feel free to try other configurations)



## Get timings

Since the primary goal of the exercise was to benchmark the code in order to analyze its scalability (both weak and strong), I prepared some bash scripts to automatise the "timings-gathering" phase. You find them in the folder [`timings/`](timings/), and it should be clear what they do refer to.

As an example, let's take into consideration the script [`timings/getdata_strong.sh`](timings/getdata_strong.sh) (same considerations apply for the other scripts). It was meant to get data about strong scalability, varying the number of processes from 1 to 128. It can be lauched with the `sbatch` command, followed by the name of the csv which will be created (and filled with numbers!)

```bash
sbatch getdata_strong.sh yourcsv_name.csv
```

Given the time limitations on the ORFEO cluster, I needed to run it twice (one loop from 1 to 64 processes and another from 65 to 128). The script is now set to perform the whole loop, but feel free to change it according to your preferences!

**Note:** the script [`timings/getdata_omp.sh`](timings/getdata_omp.sh) was used to compare the standard omp version with the "L1-optimized" one. If you try to run it, you will get timings about the "L1-optimized" version. If you want to get timings with the original version, you will need to change a bit the source code an then re-build the executable. More precisely, once open the file [`source/hybrid_qsort.c`](source/hybrid_qsort.c), you just need to uncomment lines 208 and 391 and comment the lines 207 and 390.

