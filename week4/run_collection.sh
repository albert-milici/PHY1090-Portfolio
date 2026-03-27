#!/bin/bash
# benchmarks collection methods for vector addition

OUTPUT="week4/output/collection_results.csv"
NP=8

# compiles each variant if binary is missing or older than source
for method in sendrecv gather reduce; do

    SRC="week4/src/vector_collect_$method.c"
    BIN="bin/vector_collect_$method"
    if [ ! "$BIN" -nt "$SRC" ]; then
        echo "Compiling $SRC..."
        mpicc "$SRC" -o "$BIN"
    fi
done

echo "method,vector_size,collect_time_s" > "$OUTPUT"

for size in 1000 10000 100000 1000000 10000000; do

    for method in sendrecv gather reduce; do
        result=$(mpirun -n $NP ./bin/vector_collect_$method $size)
        collect_time=$(echo "$result" | grep -oP 'time: \K[0-9.]+')
        echo "$method,$size,$collect_time" >> "$OUTPUT"
        
    done
done

echo "Results saved to $OUTPUT"