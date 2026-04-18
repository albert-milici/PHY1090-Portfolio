# Week 9: Quantum Computer Simulation with GPU

## How to build and run

```bash
python3 week9/src/grover_gpu.py
```

Requires `numpy` and `pytorch`. Works on CPU if no GPU is available. 

---

## GPU Simulation

This code follows Part 3 of the [ENCCS tutorial](https://enccs.se/tutorials/2024/02/quantum-computing-for-beginners-part-3-gpus/), the quantum simulator was modified to use PyTorch instead of numpy. This allows the workspace to live in GPU memory, where the matrix operations can be accelerated.

The core change is replacing numpy calls with PyTorch equivalents (`pt.reshape`, `pt.kron`, `pt.matmul`, `pt.swapaxes`). The workspace is created on either the GPU using CUDA or CPU. Gate matrices are converted to PyTorch tensors on the same device as the workspace before multiplication.

Grover's search with 16 qubits was benchmarked on both GPU and CPU. The GPU version runs significantly faster because the workspace at 16 qubits contains 2^16 = 65,536 elements, and the matrix operations parallelise well on GPU hardware.

---

## Files

| File | Description |
|------|------------|
| `src/grover_gpu.py` | PyTorch quantum simulator with GPU/CPU Grover's search benchmark |