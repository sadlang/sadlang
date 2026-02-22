# 🏎️ Performance Benchmarks for Sad Language

This directory contains performance benchmarks for the Sad language compiler and runtime.

## 📁 Structure

```
benchmarks/
├── README.md              # This file
├── micro/                 # Micro-benchmarks (small operations)
│   ├── arithmetic.s       # Basic arithmetic operations
│   ├── arrays.s           # Array operations
│   ├── strings.s          # String operations
│   └── functions.s        # Function call overhead
├── macro/                 # Macro-benchmarks (real-world scenarios)
│   ├── fibonacci.s        # Recursive vs iterative
│   ├── sorting.s          # Various sorting algorithms
│   ├── json_parse.s       # JSON parsing performance
│   └── http_server.s      # HTTP request handling
├── memory/                # Memory benchmarks
│   ├── allocation.s       # Memory allocation patterns
│   ├── gc_pressure.s      # Garbage collection stress test
│   └── ownership.s        # Ownership system overhead
└── results/               # Benchmark results (JSON)
    └── baseline.json
```

## 🚀 Running Benchmarks

### All Benchmarks
```bash
sad benchmark --all
```

### Specific Category
```bash
sad benchmark --category=micro
```

### Single Benchmark
```bash
sad benchmark benchmarks/micro/arithmetic.s
```

### With Iterations
```bash
sad benchmark --iterations=100 --warmup=10 benchmarks/micro/arithmetic.s
```

## 📊 Output Format

Results are saved in JSON format:

```json
{
  "benchmark": "arithmetic_addition",
  "iterations": 1000000,
  "total_time_ms": 45.32,
  "avg_time_ns": 45.32,
  "min_time_ns": 42.1,
  "max_time_ns": 89.5,
  "std_dev_ns": 3.2,
  "ops_per_second": 22067000
}
```

## 🎯 Baseline Targets

| Benchmark | Target | Notes |
|-----------|--------|-------|
| Integer addition | < 1ns | Comparable to C |
| Function call | < 5ns | Minimal overhead |
| Array access | < 2ns | O(1) access |
| String concat | < 100ns | For short strings |
| JSON parse (1KB) | < 1ms | Streaming parser |
| HTTP req/res | < 1ms | Basic GET request |

## 🔧 Benchmark Guidelines

1. **Warm-up**: Always include warm-up iterations
2. **Iterations**: Run enough iterations for statistical significance
3. **Isolation**: Test one thing at a time
4. **Consistency**: Run on the same machine/environment
5. **Documentation**: Document what's being measured

## 📈 Tracking Performance

Compare against baseline:
```bash
sad benchmark --compare=baseline.json --all
```

This will show:
- ✅ Improvements (>5% faster)
- ⚠️ Regressions (>5% slower)
- ➡️ No significant change
