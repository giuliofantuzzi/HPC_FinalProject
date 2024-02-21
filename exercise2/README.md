# HPC Final Project: Exercise 2B

## Table of contents
- [Project structure](#project-structure)

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
