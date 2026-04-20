# PHY1090 High Performance and Quantum Computing — Practical Portfolio

## Overview

This repository contains the practical work for PHY1090 High Performance and Quantum Computing. The first half covers high performance computing with C and MPI, progressing from basic benchmarking through to a parallelised physics simulation. The second half covers quantum computing, building a quantum computer simulator in Python.

Each week's work is in its own directory with a README explaining what was done, how to run it, and what the results were.

## Structure

| Directory | Topic |
|-----------|-------|
| `week1/`  | Introduction — basic MPI setup and proof of concept |
| `week2/`  | Performance and Parallelism — benchmarking C vs Python, internal timing, file I/O |
| `week3/`  | Introduction to MPI — hello world, serial vs parallel vector addition |
| `week4/`  | MPI Communications — send types, latency/bandwidth measurement, collective operations |
| `week5/`  | Communicators and Topologies — string wave simulation, MPI parallelisation, spring force model |
| `week7/`  | Quantum Computing — quantum computer simulator with gates and measurements |
| `week8/`  | Grover's Search — amplitude amplification and phase oracles |
| `week9/`  | GPU Simulation — PyTorch quantum simulator with GPU acceleration |

## Hardware

Code was developed and tested on two machines:

**University server:** 2x Intel Xeon E5-2620 v4 (8 cores each, 16 cores total, 32 threads, up to 3.0 GHz), 2 sockets, 2 NUMA nodes, 32 GB RAM. Open MPI 4.0.3. 

**Local machine:** AMD Ryzen 7 7800X3D (8 cores, 16 threads, up to 5.0 GHz), 1 socket, 1 NUMA node, 32 GB RAM. Open MPI 4.1.6. Used for development and benchmarking.


## How to build and run

Each week's README contains specific build and run instructions. In general, C programs are compiled from the repo root:

```bash
gcc week2/src/hello_world.c -o bin/hello_world
mpicc week4/src/pingpong.c -o bin/pingpong
mpirun -n 4 ./bin/pingpong 1000
```

Python programs run directly:

```bash
python3 week7/src/qc_simulator.py
```

Bash scripts handle compilation automatically and are run from the repo root:

```bash
./week4/run_latency.sh
./week5/run_benchmark.sh
```

## Summary of results

**Week 2-3:** C consistently outperformed Python for simple benchmarks. MPI overhead dominated runtime for lightweight tasks, with no crossover found for vector addition up to 10 million elements.

**Week 4:** Measured shared-memory MPI latency at ~210 ns and bandwidth at ~6900 MB/s. Collective operations (Scatter, Gather) outperformed manual Send/Recv by 2-4x for distribution and collection.

**Week 5:** Parallelised a vibrating string simulation using MPI with spatial decomposition. The simple copy-based model showed no speedup at any scale because the per-element computation was too cheap relative to the per-timestep Gather cost. An improved spring force model with Hooke's law, damping, and Euler integration increased computation per point by 5-10x but still produced no meaningful speedup. Testing on both the local workstation and the dual-socket server confirmed the bottleneck is the all-to-one communication pattern, not the hardware architecture.

**Week 7-9:** Built a quantum computer simulator following the [ENCCS tutorial](https://enccs.se/tutorials/2024/02/quantum-computing-for-beginners-part-1/). Implemented qubit stack operations, standard gates, and named qubit references. Applied Grover's search algorithm to demonstrate quadratic speedup. Converted the simulator to PyTorch for GPU acceleration.
