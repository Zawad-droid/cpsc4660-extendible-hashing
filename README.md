
# Extendible Hashing for Dynamic Files

CPSC 4660 — Database Management Systems  
University of Lethbridge

## Overview

This project implements the core extendible hashing algorithm in C++17, based on the method introduced by Fagin et al. (1979).

The implementation supports:

- Record insertion and lookup
- Updating existing keys
- Fixed-capacity buckets
- Bucket splitting and record redistribution
- Directory doubling
- Global and local depth management

The implementation uses in-memory storage.

## Building and Running

Requires CMake and a C++17-compatible compiler.

Build the project:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the main program containing 32 correctness checks:

```bash
./build/extendible_hash
```

Run the directory splitting demonstration:

```bash
./build/extendible_hash_demo
```

The demonstration shows the directory before and after a bucket split, including changes in global depth, directory size, bucket splits, and directory doublings. It also verifies that the inserted records remain accessible.

Run the integration tests:

```bash
ctest --test-dir build --output-on-failure -V
```

Run the performance benchmark:

```bash
./build/extendible_hash_benchmark
```

The benchmark generates `benchmark_results.csv` in the current working directory.

## Performance Evaluation

The benchmark evaluates four dataset sizes:

- 1,000 records
- 10,000 records
- 50,000 records
- 100,000 records

Each dataset is tested with bucket capacities of 4, 16, and 64 records.

The following measurements are collected:

- Insertion time
- Lookup time
- Number of bucket splits
- Number of directory doublings
- Number of allocated buckets
- Directory size
- Average bucket occupancy

Synthetic integer keys are shuffled using a fixed random seed to ensure reproducible insertion order.

The benchmark was executed three times. Median insertion and lookup times were calculated from these runs to reduce the influence of timing fluctuations.

The `results/` directory contains:

- `benchmark_run1.csv`
- `benchmark_run2.csv`
- `benchmark_run3.csv`
- `benchmark_median_results.csv`

The median results are used in the graphs presented in the final project report.

## Reference

Fagin, R., Nievergelt, J., Pippenger, N., & Strong, H. R. (1979). Extendible hashing—A fast access method for dynamic files. *ACM Transactions on Database Systems, 4*(3), 315–344.
