# Week 8: Grover's Search

## How to build and run

```bash
python3 week8/src/grover_search.py
```

Requires `numpy`.

---

## Grover's Search

Building on the quantum simulator from week 7, Grover's search algorithm was implemented following Part 2 of the [ENCCS tutorial](https://enccs.se/tutorials/2024/02/quantum-computing-for-beginners-part-2-grovers-search/). Grover's search finds a specific input to a function in O(√N) time compared to O(N) for a classical search, giving a quadratic speedup.

The algorithm uses two phase oracles. The sample oracle marks the solution by flipping the sign of its amplitude, and the zero oracle marks the all zeros state. By alternating these oracles with Hadamard gates, the amplitude of the solution is gradually amplified over several iterations. The number of iterations needed is approximately π/4 × √(2^n). For 6 qubits this gives 5 iterations, after which the correct answer (111101) is measured with high probability.

---

## Files

| File | Description |
|------|------------|
| `src/grover_search.py` | Quantum simulator with Grover's search algorithm |