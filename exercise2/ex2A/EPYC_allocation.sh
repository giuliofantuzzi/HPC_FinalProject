#!/bin/bash
salloc -n 128 -N 1 -p EPYC --time=01:30:00 -J "HPC_exam" -A dssc
