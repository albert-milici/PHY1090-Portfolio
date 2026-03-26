#!/bin/bash
# runs pingpong_bw with varying array sizes and outputs CSV

OUTPUT="week4/bandwidth_results.csv"
NUM_PINGS=1000

echo "size_bytes,counter,elapsed_s,average_s" > "$OUTPUT"

for size in 8 36 64 288 512 2304 4096 18432 32768 147456 262144 635760 1048576; do

    result=$(mpirun -n 2 ./bin/pingpong_bandwidth $NUM_PINGS $size)
    counter=$(echo "$result" | grep -oP 'Counter: \K[0-9]+')
    elapsed=$(echo "$result" | grep -oP 'Elapsed: \K[0-9.]+')
    average=$(echo "$result" | grep -oP 'Average: \K[0-9.]+')
    echo "$size,$counter,$elapsed,$average" >> "$OUTPUT"

done

echo "Results saved to $OUTPUT"