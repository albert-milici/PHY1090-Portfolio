# Week 7: Quantum Computing Simulator

## How to build and run

```bash
python3 week7/src/qc_simulator.py
```

Requires `numpy`.

---

## Quantum Computer Simulator

This project follows the [ENCCS "Quantum Computing for Beginners"](https://enccs.se/tutorials/2024/02/quantum-computing-for-beginners-part-1/) tutorial, a quantum computer simulator was built in Python using numpy. The simulator represents a quantum computer as a stack machine where qubits are pushed onto a workspace, gate operations are applied via matrix multiplication, and qubits are measured by calculating probabilities and randomly drawing a result.

The simulator implements four core operations, `pushQubit` which expands the workspace using the Kronecker product, `applyGate` which performs matrix multiplication on the workspace, `tosQubit` which moves a qubit to the top of the stack by permuting axes, and `measureQubit` which collapses a qubit by drawing 0 or 1 according to its probability and shrinking the workspace. Standard gates are defined including Pauli X/Y/Z, Hadamard, phase gates, CNOT, SWAP, and Toffoli.

The simulator was then improved by adding a name stack so qubits can be referenced by name instead of position. This allows gate operations to specify which qubits to act on by name, and `tosQubit` to move any named qubit to the top of the stack automatically. Controlled gates were also added, including a multi control Toffoli gate that uses ancilla qubits with proper uncomputing to avoid entanglement issues.

---

## Files

| File | Description |
|------|------------|
| `src/qc_simulator.py` | Quantum computer simulator with gates, named qubits, and controlled operations |