#!/bin/bash
# benchmarks serial vs parallel improved string wave model

OUTPUT="week5/output/benchmark_improved_results.csv"
SRC_SERIAL="week5/src/string_wave_improved.c"
SRC_MPI="week5/src/string_wave_mpi_improved.c"
BIN_SERIAL="bin/string_wave_improved"
BIN_MPI="bin/string_wave_mpi_improved"
CYCLES=5
SAMPLES=400

# compiles serial if needed
if [ ! "$BIN_SERIAL" -nt "$SRC_SERIAL" ]; then
    echo "Compiling serial..."
    gcc "$SRC_SERIAL" -o "$BIN_SERIAL" -lm
fi

# compiles MPI if needed
if [ ! "$BIN_MPI" -nt "$SRC_MPI" ]; then
    echo "Compiling MPI..."
    mpicc "$SRC_MPI" -o "$BIN_MPI" -lm
fi

echo "method,num_processes,points,elapsed_s" > "$OUTPUT"

for points in 48 240 480 2400 4800 24000 48000; do
    echo "Testing $points points..."

    # serial run
    result=$(./$BIN_SERIAL $points $CYCLES $SAMPLES /dev/null)
    elapsed=$(echo "$result" | grep -oP 'Elapsed: \K[0-9.]+')
    echo "serial,1,$points,$elapsed" >> "$OUTPUT"

    # parallel runs with 2, 4, 8 processes
    for np in 2 4 8; do
        result=$(mpirun -n $np ./$BIN_MPI $points $CYCLES $SAMPLES /dev/null)
        elapsed=$(echo "$result" | grep -oP 'Elapsed: \K[0-9.]+')
        echo "mpi,$np,$points,$elapsed" >> "$OUTPUT"
    done
done

echo "Results saved to $OUTPUT"