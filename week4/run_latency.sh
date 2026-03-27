#!/bin/bash
# runs pingpong with varying num_pings and outputs CSV

OUTPUT="week4/output/latency_results.csv"
SRC="week4/src/pingpong.c"
BIN="bin/pingpong"

# compiles if binary is missing or older than source
if [ ! "$BIN" -nt "$SRC" ]; then
    echo "Compiling $SRC..."
    mpicc "$SRC" -o "$BIN"
fi

echo "num_pings,counter,elapsed_s,average_s" > "$OUTPUT"

for n in 10 100 1000 10000 100000 1000000; do
    result=$(mpirun -n 2 ./$BIN $n)
    counter=$(echo "$result" | grep -oP 'Counter: \K[0-9]+')
    elapsed=$(echo "$result" | grep -oP 'Elapsed: \K[0-9.]+')
    average=$(echo "$result" | grep -oP 'Average: \K[0-9.]+')
    echo "$n,$counter,$elapsed,$average" >> "$OUTPUT"
done

echo "Results saved to $OUTPUT"