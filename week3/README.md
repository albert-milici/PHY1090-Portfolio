# Week 3: Introduction to MPI

## How to build and run

Compile the C programs from the repo root with the commands below. This places the compiled binaries in the top-level `bin/` directory.

```bash

mpicc week3/hello_mpi.c -o bin/hello_mpi
gcc week3/hello_serial.c -o bin/hello_serial
gcc week3/vector_serial.c -o bin/vector_serial 
mpicc week3/vector_mpi.c -o bin/vector_mpi
```

Run the hello world programs with:

```bash
mpirun -np 4 bin/hello_mpi
./bin/hello_serial 4
```

Run the vector addition programs with:

```bash
./bin/vector_serial 10
mpirun -np 4 bin/vector_mpi 10
```

To benchmark the whole program externally, use the Linux `time` command. For example:

```bash
time mpirun -np 4 bin/hello_mpi
time ./bin/hello_serial 4
time ./bin/vector_serial 1000000
time mpirun -np 4 bin/vector_mpi 1000000
```
Run bash scripts from the repo root:

```bash
./week3/experiment_1.sh
./week3/run_benchmark.sh
```
Note if a script gives a permission error, run `chmod +x week4/*.sh` first.


---

## Part 1: MPI "Hello World"

### Step 1: Implement and run an MPI hello world program

A simple MPI hello world program was created in `hello_mpi.c`. Each process initialises MPI, gets its rank and the communicator size, and prints a message of the form `Hello, I am [rank] of [size]`. Running with multiple processes shows that each rank executes the same program independently, with MPI providing the rank and size information that distinguishes them.

As a quick experiment, I also tried breaking the usual MPI ordering rules by moving or omitting `MPI_Init()` before other MPI calls. This broke the code, confirming that MPI must be initialised before functions such as `MPI_Comm_rank()` and `MPI_Comm_size()` are used.

### Step 2: Benchmark the hello world program

The MPI hello world program was benchmarked with different numbers of processes. 

*Table 1. MPI processes benchmark results*

| Processes | Real Time | User Time | System Time |
|-----------|-----------|-----------|-------------|
| 1         | 0m0.710s  | 0m0.035s  | 0m0.288s    |
| 2         | 0m0.678s  | 0m0.058s  | 0m0.281s    |
| 3         | 0m0.729s  | 0m0.074s  | 0m0.356s    |
| 4         | 0m0.736s  | 0m0.090s  | 0m0.386s    |
| 5         | 0m0.784s  | 0m0.100s  | 0m0.499s    |
| 6         | 0m0.811s  | 0m0.126s  | 0m0.550s    |
| 7         | 0m0.830s  | 0m0.131s  | 0m0.590s    |
| 8         | 0m0.871s  | 0m0.148s  | 0m0.676s    |

Table 1. shows the results ,on my local machine I tested from 1 to 8 processes and recorded the `real`, `user`, and `sys`. As the number of processes increased, the combined `user` and `sys` time moved closer to the `real` time, showing the increasing cost of managing multiple MPI processes. In the runs recorded here it did not clearly exceed the real time, but the trend still showed that more total CPU time was being used as more processes were launched, and the system time begins to approach real time with the number of processes.

A simple serial equivalent was also created in `hello_serial.c`, which just prints the same style of message in a normal loop. This was much faster than the MPI version. For example, `time mpirun -np 1 bin/hello_mpi` took about `0.709 s`, while the serial version took about `0.001 s`. The reason is that the actual work in hello world is tiny, so the MPI startup, launcher, and communicator overhead dominate the runtime. For a task this small, parallelisation gives no performance benefit.

---

## Part 2: MPI exercise breakdown

The `proof.c` example from week 1 was reviewed in order to understand the structure of a simple MPI program. At a high level, `main()` performs the setup work, checks the arguments, initialises MPI, gets the rank and communicator size, checks that enough processes are available, decides which task each rank should carry out, and then finalises MPI. The main point of the program is not the complexity of the calculation itself, but the way it is split between root and client ranks.

The `check_task()` function is where the MPI roles split. If the rank is `0`, the root process receives messages from the client ranks and combines them into a final result. If the rank is greater than `0`, the client process performs its assigned calculation and sends the result back to root. This makes `proof.c` a useful template for later MPI programs.

---

## Part 3: MPI Vector Addition

### Step 1: Read and understand the serial code

The starting point for this part was `vector_serial.c`, which sums the elements of a vector. The original  version only used zeroes, so the result was not very informative. The serial code was therefore modified so that the vector was initialised in a non trivial way with `vector[i] = i + 1`. This gives the expected sum `n(n + 1) / 2`, which makes it easy to check correctness for smaller test cases. For example, an input of `5` should give a sum of `15`.

Internal timing was also added around the summation step so that the cost of the core loop could be measured separately from the overall program runtime. For example, running `./bin/vector_serial 1000000` gave `Sum: 1784293664` and `Runtime for sum: 0.000957 seconds`. The runtime is useful, but the sum is incorrect here because the 32-bit `int` type overflows for sufficiently large inputs.

### Step 2: Create a parallel version

A parallel MPI version was then created in `vector_mpi.c`. Each process creates and initialises the same vector, works out which chunk of the vector belongs to its rank, sums only that chunk, and then sends its partial sum back to the root process. The root rank receives all partial sums and adds them together to get the final answer. This follows the same general root/client communication structure seen earlier in `proof.c`.

### Step 3: Benchmark and compare serial and parallel implementations

The serial and MPI versions were benchmarked externally using the Linux `time` command, and the `real` times were stored in `benchmark_results.csv`.

*Table 2. Serial vs MPI vector addition benchmark results*

| Version    | Vector Size | Real Time |
|------------|-------------|-----------|
| serial     | 1000        | 0m0.001s  |
| parallel_4 | 1000        | 0m0.780s  |
| serial     | 10000       | 0m0.001s  |
| parallel_4 | 10000       | 0m0.735s  |
| serial     | 100000      | 0m0.001s  |
| parallel_4 | 100000      | 0m0.753s  |
| serial     | 1000000     | 0m0.004s  |
| parallel_4 | 1000000     | 0m0.834s  |
| serial     | 10000000    | 0m0.032s  |
| parallel_4 | 10000000    | 0m0.797s  |

Table 2 shows that for the tested range, the serial program was always much faster than the parallel one. The serial version stayed below `0.05 s` even at ten million elements, while the MPI version with 4 processes stayed roughly constant at around `0.7` to `0.8 s`.

No crossover point was found in the tested range. The MPI runtime overhead, roughly `0.75 s`, was much larger than the actual cost of summing the vector in serial, even for ten million elements. This shows that the operation is too simple and too lightweight to benefit from MPI on a small local system. In this case, communication and process management overhead outweigh any speedup from splitting the work.

Since the vector was initialised with `vector[i] = i + 1`, the expected sum is `n(n + 1) / 2`. With a 32-bit `int`, overflow occurs above about `65535`, so the larger benchmark cases were mainly useful for timing rather than for checking numerical correctness. Correctness was therefore only reliable for smaller inputs where overflow does not occur.


---

## Files

## Files

| File | What it does |
|------|-------------|
| `src/hello_mpi.c` | MPI hello world program that prints the rank and communicator size |
| `src/hello_serial.c` | Serial equivalent of the hello world program used for comparison |
| `src/vector_serial.c` | Serial vector addition program |
| `src/vector_mpi.c` | Parallel MPI vector addition program t |
| `experiment_1.sh` | Bash script to benchmark MPI hello world for different numbers of processes |
| `run_benchmark.sh` | Bash script to benchmark serial and MPI vector addition and save the real times |
| `output/experiment_1.csv` | Benchmark results for MPI hello world with different process counts |
| `output/benchmark_results.csv` | Benchmark results comparing serial and MPI vector addition |
| `README.md` | Summary of the week 3 work, methods, results, and conclusions |

