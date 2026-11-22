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

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the main program:

```bash
./build/extendible_hash
```

Run the integration tests:

```bash
ctest --test-dir build --output-on-failure
```

Run the performance benchmark:

```bash
./build/extendible_hash_benchmark
```

The benchmark generates `benchmark_results.csv` in the current working directory.

## Performance Evaluation

The benchmark evaluates datasets containing 1,000, 10,000, 50,000, and 100,000 records, using bucket capacities of 4, 16, and 64.

It measures insertion time, lookup time, bucket splits, directory doublings, directory size, allocated buckets, and bucket occupancy.

The `results/` directory contains three benchmark runs and their calculated median results.

## Reference

Fagin, R., Nievergelt, J., Pippenger, N., & Strong, H. R. (1979). Extendible hashing—A fast access method for dynamic files. *ACM Transactions on Database Systems, 4*(3), 315–344.