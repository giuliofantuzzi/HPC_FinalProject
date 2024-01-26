#!/bin/bash
salloc -n 128 -N 1 -p EPYC --time=00:40:00 -J "HPC_exam" -A dssc
