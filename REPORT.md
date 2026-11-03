# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.459 | 1.120 | 59.0% faster | 15.939 |
| heavy-matching | 10,000 | 0.568 | 1.219 | 53.4% faster | 27.276 |
| mixed | 10,000 | 0.514 | 1.289 | 60.1% faster | 22.718 |
| modification-heavy | 10,000 | 0.607 | 1.542 | 60.7% faster | 27.169 |
| mostly-resting | 10,000 | 0.457 | 1.277 | 64.2% faster | 22.641 |
| mixed | 100,000 | 5.503 | 14.998 | 63.3% faster | 260.047 |
| mixed | 1,000,000 | 62.821 | 262.407 | 76.1% faster | 3507.347 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-11-03T21:00:47.766946+00:00.
Correctness passed. Build cache: 9/9 commands reused. Correctness cache: 32/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.459 | 0.004 | 0.430 to 0.469 | 0.0235 | 21,788,486 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.120 | 0.015 | 1.079 to 1.177 | 0.0215 | 8,925,583 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 15.939 | 0.196 | 15.673 to 17.741 | 0.0425 | 627,374 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.568 | 0.012 | 0.533 to 0.580 | 0.0276 | 17,613,386 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.219 | 0.010 | 1.159 to 1.253 | 0.0201 | 8,203,445 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 27.276 | 0.206 | 26.381 to 30.952 | 0.0455 | 366,623 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.514 | 0.035 | 0.475 to 0.647 | 0.0975 | 19,436,346 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.289 | 0.034 | 1.226 to 1.697 | 0.0922 | 7,757,952 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 22.718 | 1.219 | 20.710 to 25.970 | 0.0682 | 440,174 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.607 | 0.055 | 0.520 to 0.788 | 0.1179 | 16,482,366 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.542 | 0.055 | 1.471 to 1.711 | 0.0414 | 6,483,508 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 27.169 | 1.173 | 25.698 to 30.110 | 0.0492 | 368,071 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.457 | 0.015 | 0.432 to 0.480 | 0.0342 | 21,877,865 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.277 | 0.031 | 1.214 to 1.358 | 0.0338 | 7,833,405 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 22.641 | 1.098 | 21.184 to 25.002 | 0.0506 | 441,672 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 5.503 | 0.068 | 5.374 to 6.254 | 0.0499 | 18,171,219 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.998 | 0.121 | 14.828 to 15.281 | 0.0096 | 6,667,333 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 260.047 | 6.393 | 251.120 to 287.368 | 0.0411 | 384,546 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 62.821 | 1.096 | 61.203 to 76.423 | 0.0599 | 15,918,244 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 262.407 | 26.454 | 229.730 to 309.908 | 0.1034 | 3,810,876 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3507.347 | 179.449 | 3064.848 to 3808.773 | 0.0666 | 285,116 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 19801.398 | no |
| warmup | 18145.548 | no |
| setup | 44.438 | no |
| replay | 62809.132 | yes |
| verify | 45831.251 | no |
| disposal | 2239.679 | no |
| reporting | 3376.076 | no |
| measurement stage wall | 168571.823 | no |
| orchestration residual | 16324.301 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `04f73d2fce2c362c7bdd7315113bc78ba13086d094cbf4646fcd1714f4fd5f2a`. Saved source ID: `2fad7976e85a6ea7e74300b0e234fe2797611c4d515d1a0e45781ea7aeda22ab`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
