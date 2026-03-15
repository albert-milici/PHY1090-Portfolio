#!/bin/bash
# benchmarks serial vs parallel vector addition

OUTPUT="week3/benchmark_results.csv"

echo "version,vector_size,real_time" > "$OUTPUT"

# compiles both versions
gcc week3/vector_serial.c -o bin/vector_serial
mpicc week3/vector_mpi.c -o bin/vector_mpi

for size in 1000 10000 100000 1000000 10000000; do

    # times the serial version
    serial_time=$( { time ./bin/vector_serial $size; } 2>&1 | grep real | awk '{print $2}')
    echo "serial,$size,$serial_time" >> "$OUTPUT"

    # times the parallel version with 4 processes
    parallel_time=$( { time mpirun -n 4 ./bin/vector_mpi $size; } 2>&1 | grep real | awk '{print $2}')
    echo "parallel_4,$size,$parallel_time" >> "$OUTPUT"
    
done

echo "Results saved to $OUTPUT"