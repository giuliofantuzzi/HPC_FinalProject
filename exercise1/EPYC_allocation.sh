#!/bin/bash
salloc -n256 -N2 -p EPYC --time=02:00:00 -A dssc
