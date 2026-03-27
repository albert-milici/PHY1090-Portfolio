#!/bin/bash
# benchmarks distribution methods for vector addition

OUTPUT="week4/output/distribution_results.csv"
NP=4

# compiles each variant if binary is missing or older than source
for method in bcast scatter diy; do

    SRC="week4/src/vector_mpi_$method.c"
    BIN="bin/vector_mpi_$method"
    if [ ! "$BIN" -nt "$SRC" ]; then
        echo "Compiling $SRC..."
        mpicc "$SRC" -o "$BIN"
    fi
done

echo "method,vector_size,dist_time_s" > "$OUTPUT"

for size in 1000 10000 100000 1000000 10000000; do

    for method in bcast scatter diy; do

        result=$(mpirun -n $NP ./bin/vector_mpi_$method $size)
        dist_time=$(echo "$result" | grep -oP 'time: \K[0-9.]+')
        echo "$method,$size,$dist_time" >> "$OUTPUT"

    done
done

echo "Results saved to $OUTPUT"