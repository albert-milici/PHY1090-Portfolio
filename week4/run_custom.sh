#!/bin/bash
# benchmarks built-in vs custom reduce

OUTPUT="week4/custom_reduce_results.csv"
NP=4

mpicc week4/vector_collect_reduce.c -o bin/vector_collect_reduce
mpicc week4/vector_collect_custom_reduce.c -o bin/vector_collect_custom_reduce

echo "method,vector_size,collect_time_s" > "$OUTPUT"

for size in 1000 10000 100000 1000000 10000000; do

    for run in 1 2 3; do
        builtin_time=$(mpirun -n $NP ./bin/vector_collect_reduce $size | grep -oP 'time: \K[0-9.]+')
        echo "builtin,$size,$builtin_time" >> "$OUTPUT"

        custom_time=$(mpirun -n $NP ./bin/vector_collect_custom_reduce $size | grep -oP 'time: \K[0-9.]+')
        echo "custom,$size,$custom_time" >> "$OUTPUT"
        
    done
done

echo "Results saved to $OUTPUT"