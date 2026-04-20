# Week 2: Performance and Parallelism

## How to build and run

Compile the C programs from the repo root with the commands below. Assumes the `bin` and `data` directories already exist.

```bash
gcc week2/src/hello_world.c -o bin/hello_world
gcc week2/src/repeat_adder.c -o bin/repeat_adder
gcc week2/src/time_print.c -o bin/time_print -std=c11
gcc week2/src/time_write.c -o bin/time_write -std=c11
gcc week2/src/time_read.c -o bin/time_read -std=c11
```
Note `-std=c11` might be needed without it, timespec_get may produce a warning or error depending on your gcc version's default standard.

Run the Python programs with:

```bash
python3 week2/src/hello_world.py
python3 week2/src/repeat_adder.py 2 3
python3 week2/src/time_print.py 10
python3 week2/src/time_write.py 1000 data/py_output.txt
python3 week2/src/time_read.py data/py_output.txt
```

To benchmark the whole program externally, use the Linux `time` command. For example:

```bash
time ./bin/hello_world
time python3 week2/src/hello_world.py
```

---

## Part 1: Introducing the time command

The `time` command reports three quantities, `real`, `user`, and `sys`. The `real` value is the total wall clock time, `user` is the CPU time spent running user code, and `sys` is the CPU time spent in system calls. This command was used throughout the exercise to compare the overall runtime of the C and Python programs.

---

## Part 2: Benchmarking with time

### Step 1: Compile the sample C code

The provided C programs were copied into `week2/src/` and compiled into the top-level `bin/` directory so that source files and binaries remained separate. This follows the same directory structure used in the rest of the portfolio.

### Step 2: Time both C and Python "Hello World" programs

The two hello world programs both completed essentially instantly, but the Python version was slower overall. The C version took about `0.001 s`, while the Python version took about `0.009 s`. For a program this small, the main reason is that Python has interpreter startup overhead, while the compiled C binary runs directly.

### Step 3: Time C and Python "Repeat Adder" programs

Both versions worked correctly for small inputs such as `2 3`, returning `6`. For small and medium inputs the runtime stayed very short, but large inputs showed two important behaviours. First, the runtime depends mainly on the first argument, because that value controls the number of loop iterations. Second, the C version runs into integer overflow near the 32-bit `int` limit. For example, `./bin/repeat_adder 2147483647 1` returned `2147483647` and took about `1.373 s`, while `./bin/repeat_adder 2147483647 2` returned `-2`, showing overflow. When the input exceeded the range of `int`, as in `./bin/repeat_adder 2147483648 1`, the behaviour became incorrect and the program returned `0`. Overall, C was faster than Python, but the main lesson was that runtime follows the structure of the algorithm. In this case, the loop count matters much more than the final numerical result.

---

## Part 3: Benchmarking Internally

### Step 1: Examine printing to the screen

The `time_print` programs were used to place timing code around the loop itself instead of timing the whole program externally. For an input of `10`, the C version reported `0.000020 seconds` and the Python version reported `6.914e-06 seconds`. These timings are extremely small and somewhat noisy, so they should not be over interpreted. The main point of this part was to demonstrate that internal timing can be used to measure a specific section of code. Printing is still an I/O task, and as the amount of output increases it becomes much more significant than the loop arithmetic itself.

---

## Part 4: Modify the code

### Step 1: Experiment with writing to a file

The `time_write` programs were created by modifying the print benchmark so that it wrote numbers to a file instead of sending them to the terminal. In C, writing `100` lines took `0.000024 s`, writing `1000` lines took `0.000048 s`, and writing `5000` lines took `0.000172 s`. In Python, writing `100` lines took `0.000119 s`, writing `1000` lines gave results between about `0.000205 s` and `0.000946 s`, and writing `5000` lines took `0.000728 s`. C was faster in these tests, and the write time broadly increased with file size. The smallest Python timings were a little noisy because the files were very small and the measured runtimes were close to the timer resolution.

### Step 2: Experiment with reading from a file

The `time_read` programs were then used to read the generated files into memory. For a file of `3890` bytes, the C version took `0.000003 s` and the Python version took `2.289e-05 s`. C was again faster in this simple test. Reading is still an I/O task, and larger files would be expected to take longer.

### Step 3: Evaluate the results

Overall, C was consistently faster than Python for these simple benchmarks. For very small programs, Python startup time can dominate the measured runtime. The repeat adder example showed that runtime depends mainly on the structure of the algorithm rather than just the final answer. The printing, writing, and reading tests also showed that I/O operations can dominate runtime much more than basic arithmetic. Finally, the large input tests confirmed that C `int` values overflow once the input goes beyond the 32-bit range.

---

## Files

| File | What it does |
|------|-------------|
| `hello_world.c` | C hello world program |
| `hello_world.py` | Python hello world program |
| `repeat_adder.c` | C repeated addition program |
| `repeat_adder.py` | Python repeated addition program |
| `time_print.c` | C program with internal timing around terminal output |
| `time_print.py` | Python program with internal timing around terminal output |
| `time_write.c` | C program to time writing numbers to a file |
| `time_write.py` | Python program to time writing numbers to a file |
| `time_read.c` | C program to time reading a file into memory |
| `time_read.py` | Python program to time reading a file into memory |

