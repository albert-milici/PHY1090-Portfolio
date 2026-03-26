#!/bin/bash
# runs pingpong with varying num_pings and outputs CSV

OUTPUT="week4/latency_results.csv"

echo "num_pings,counter,elapsed_s,average_s" > "$OUTPUT"

for n in 10 100 1000 10000 100000 1000000; do
    result=$(mpirun -n 2 ./bin/pingpong $n)
    counter=$(echo "$result" | grep -oP 'Counter: \K[0-9]+')
    elapsed=$(echo "$result" | grep -oP 'Elapsed: \K[0-9.]+')
    average=$(echo "$result" | grep -oP 'Average: \K[0-9.]+')
    echo "$n,$counter,$elapsed,$average" >> "$OUTPUT"
done

echo "Results saved to $OUTPUT"