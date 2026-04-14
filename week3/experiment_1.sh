#!/bin/bash

# simple bash script to measure how time changes with number of processes

mpicc week3/src/hello_mpi.c -o bin/hello_mpi

echo "processes,real,user,sys" > week3/output/experiment_1.csv

# for np in $(seq 1 8); do
for np in 1 2 3 4 5 6 7 8; do

    { time mpirun -np $np bin/hello_mpi; } 2>&1 | tail -3 | awk -v np=$np '

        /real/ {real=$2}
        /user/ {user=$2}
        /sys/  {sys=$2; print np","real","user","sys}

    ' >> week3/output/experiment_1.csv
done

echo "Results saved as experiment_1.csv"
