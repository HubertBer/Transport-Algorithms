# Transport-Algorithms

A project for exploring and benchmarking multiple transport/routing algorithms.

## Repository Layout

```
Transport-Algorithms/
├── algorithms/      # C++ algorithm implementations and benchmarks
├── tools/
│   ├── parsing/     # Map → JSON conversion scripts (Python)
│   └── visualisation/ # Result visualisation scripts (Python)
├── data/
│   ├── maps/        # Raw map inputs (e.g. OSM exports)
│   └── graphs/      # Processed graph files (.json)
├── results/         # Benchmark output files
└── legacy/          # Original Odin-language prototypes
```

## Building the C++ Algorithms

Requirements: CMake ≥ 3.20, a C++20-capable compiler.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Python Tools

```sh
cd tools
pip install -r requirements.txt
```
