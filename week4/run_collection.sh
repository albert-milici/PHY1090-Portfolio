#!/bin/bash
# benchmarks collection methods for vector addition

OUTPUT="week4/collection_results.csv"
NP=8

mpicc week4/vector_collect_sendrecv.c -o bin/vector_collect_sendrecv
mpicc week4/vector_collect_gather.c -o bin/vector_collect_gather
mpicc week4/vector_collect_reduce.c -o bin/vector_collect_reduce

echo "method,vector_size,collect_time_s" > "$OUTPUT"

for size in 1000 10000 100000 1000000 10000000; do

    for method in sendrecv gather reduce; do
        result=$(mpirun -n $NP ./bin/vector_collect_$method $size)
        collect_time=$(echo "$result" | grep -oP 'time: \K[0-9.]+')
        echo "$method,$size,$collect_time" >> "$OUTPUT"

    done
done

echo "Results saved to $OUTPUT"