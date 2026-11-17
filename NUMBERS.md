# Numbers, mixed 1M (run started 2026-11-16 23:59 UTC)

Raw data: `profiles/numbers-20261116-1859/`. 20 blocks x 5 reps, binaries in shuffled order per block.

## Whole replay

| Binary | Median ms | MAD ms | Min ms | Max ms | ns/op |
| --- | ---: | ---: | ---: | ---: | ---: |
| current | 49.57 | 0.56 | 46.96 | 58.03 | 49.6 |
| floor | 11.47 | 0.16 | 10.64 | 11.74 | 11.5 |
| baseline | 215.24 | 8.24 | 203.73 | 248.95 | 215.2 |

## Distribution and drift (current)

- p10 to p90: 48.19 to 50.43 ms; min 46.96, max 58.03
- Reps more than 10% above median (3 of 100): block 6 rep 2: 58.03, block 6 rep 3: 57.42, block 10 rep 1: 55.91

Per-block medians, first 5 vs last 5 blocks:

| Binary | First blocks ms | Last blocks ms | Change |
| --- | ---: | ---: | ---: |
| current | 48.35 | 50.07 | +3.6% |
| floor | 11.01 | 11.57 | +5.1% |

If the floor stub drifts by the same fraction as the engine, the drift is machine-wide (likely thermal or clock-frequency change), not engine behaviour. A single slow rep inside a process with normal neighbours points to outside interference.

## Floor calculation

Floor = same harness linked against a stub engine that echoes each request (no matching, no fills).

- Floor: 11.47 ms / 1,000,000 ops = 11.5 ns/op
- Engine only: current - floor per block, median 38.13 ms (IQR 37.64 to 38.42) = 38.1 ns/op
- Floor share of replay: 11.47 / 49.57 = 23.1%
- Speedup vs baseline C++: 215.24 / 49.57 = 4.34x
- Subtraction assumes harness and engine costs add; out-of-order overlap makes engine-only an estimate.

## Per-operation latency

Clock ticks every 41.67 ns on Apple Silicon; values are whole ticks. Two clock reads add ~19 ns/op.

| Engine | Kind | Count | p50 ns | p95 ns | p99 ns | p99.9 ns | max ns |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| current | overall | 1000000 | 42 | 84 | 167 | 209 | 15916 |
| current | submit-limit | 526775 | 42 | 84 | 167 | 209 | 15916 |
| current | submit-market | 75010 | 42 | 125 | 167 | 250 | 7375 |
| current | cancel | 199504 | 42 | 84 | 166 | 208 | 6250 |
| current | modify | 198711 | 83 | 125 | 167 | 250 | 6542 |
| baseline | overall | 1000000 | 167 | 417 | 542 | 709 | 11023500 |
| baseline | submit-limit | 526775 | 166 | 458 | 583 | 750 | 11023500 |
| baseline | submit-market | 75010 | 167 | 458 | 584 | 750 | 2153042 |
| baseline | cancel | 199504 | 166 | 292 | 416 | 542 | 19875 |
| baseline | modify | 198711 | 209 | 375 | 500 | 667 | 21959 |
