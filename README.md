### About

Experiment with SSE / non-SSE variants of updating an AABB by rotating the original AABB. Uses picobench for micro-benchmarking.

### Results

Compiled under gcc 7.4.0 for release with optimisations disabled (-O0):

===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
       benchmark_normal * |       8 |     0.002 |     208 |      - |  4799040.2
           benchmark_simd |       8 |     0.001 |      62 |  0.301 | 15968063.9
       benchmark_normal * |      64 |     0.013 |     205 |      - |  4876933.6
           benchmark_simd |      64 |     0.004 |      58 |  0.285 | 17112299.5
       benchmark_normal * |     512 |     0.100 |     194 |      - |  5128667.4
           benchmark_simd |     512 |     0.031 |      61 |  0.315 | 16269980.0
       benchmark_normal * |    4096 |     0.838 |     204 |      - |  4885630.2
           benchmark_simd |    4096 |     0.251 |      61 |  0.299 | 16328548.3
       benchmark_normal * |    8192 |     1.669 |     203 |      - |  4907605.0
           benchmark_simd |    8192 |     0.474 |      57 |  0.284 | 17265216.7
===============================================================================
