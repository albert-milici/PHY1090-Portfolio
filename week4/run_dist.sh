#!/bin/bash
# benchmarks distribution methods for vector addition

OUTPUT="week4/distribution_results.csv"
NP=4

mpicc week4/vector_mpi.c -o bin/vector_mpi
mpicc week4/vector_mpi_bcast.c -o bin/vector_mpi_bcast
mpicc week4/vector_mpi_scatter.c -o bin/vector_mpi_scatter
mpicc week4/vector_mpi_diy.c -o bin/vector_mpi_diy

echo "method,vector_size,dist_time_s" > "$OUTPUT"

for size in 1000 10000 100000 1000000 10000000; do

    for method in bcast scatter diy; do
        result=$(mpirun -n $NP ./bin/vector_mpi_$method $size)
        dist_time=$(echo "$result" | grep -oP 'time: \K[0-9.]+')
        echo "$method,$size,$dist_time" >> "$OUTPUT"
        
    done
done
echo "Results saved to $OUTPUT"