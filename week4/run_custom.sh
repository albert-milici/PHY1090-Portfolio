#!/bin/bash
# benchmarks built-in vs custom reduce

OUTPUT="week4/output/custom_reduce_results.csv"
NP=4

# compiles each variant if binary is missing or older than source
for name in vector_collect_reduce vector_collect_custom_reduce; do

    SRC="week4/src/$name.c"
    BIN="bin/$name"
    if [ ! "$BIN" -nt "$SRC" ]; then
        echo "Compiling $SRC..."
        mpicc "$SRC" -o "$BIN"
    fi
done

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