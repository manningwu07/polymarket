# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.381 | 1.099 | 65.3% faster | 15.825 |
| heavy-matching | 10,000 | 0.454 | 1.143 | 60.3% faster | 26.813 |
| mixed | 10,000 | 0.427 | 1.229 | 65.2% faster | 21.212 |
| modification-heavy | 10,000 | 0.470 | 1.449 | 67.6% faster | 25.253 |
| mostly-resting | 10,000 | 0.405 | 1.167 | 65.3% faster | 21.716 |
| mixed | 100,000 | 4.482 | 14.343 | 68.8% faster | 253.102 |
| mixed | 1,000,000 | 50.461 | 218.946 | 77.0% faster | 3282.844 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-11-16T23:18:42.000697+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.381 | 0.008 | 0.362 to 0.404 | 0.0270 | 26,252,507 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.099 | 0.056 | 1.033 to 1.292 | 0.0734 | 9,101,599 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 15.825 | 0.108 | 15.338 to 17.662 | 0.0428 | 631,907 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.454 | 0.008 | 0.434 to 0.469 | 0.0233 | 22,034,537 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.143 | 0.030 | 1.089 to 1.209 | 0.0314 | 8,751,134 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 26.813 | 0.375 | 26.361 to 30.050 | 0.0444 | 372,956 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.427 | 0.004 | 0.419 to 0.440 | 0.0126 | 23,394,057 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.229 | 0.032 | 1.171 to 1.286 | 0.0304 | 8,138,074 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 21.212 | 0.595 | 20.611 to 23.625 | 0.0534 | 471,429 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.470 | 0.007 | 0.462 to 0.488 | 0.0169 | 21,274,740 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.449 | 0.031 | 1.360 to 1.511 | 0.0311 | 6,899,526 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 25.253 | 0.309 | 24.709 to 27.348 | 0.0338 | 395,999 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.405 | 0.009 | 0.384 to 0.503 | 0.0647 | 24,686,238 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.167 | 0.021 | 1.115 to 1.266 | 0.0317 | 8,570,207 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.716 | 0.285 | 21.307 to 23.996 | 0.0474 | 460,488 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 4.482 | 0.026 | 4.318 to 6.681 | 0.1201 | 22,313,748 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.343 | 0.170 | 13.996 to 14.585 | 0.0131 | 6,971,900 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 253.102 | 4.100 | 246.391 to 284.519 | 0.0493 | 395,097 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 50.461 | 0.243 | 49.611 to 55.516 | 0.0306 | 19,817,481 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 218.946 | 9.675 | 205.790 to 239.780 | 0.0486 | 4,567,332 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3282.844 | 78.953 | 2963.525 to 3540.521 | 0.0563 | 304,614 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18025.587 | no |
| warmup | 16946.151 | no |
| setup | 42.948 | no |
| replay | 59099.844 | yes |
| verify | 42846.109 | no |
| disposal | 2192.234 | no |
| reporting | 3268.437 | no |
| measurement stage wall | 158056.309 | no |
| orchestration residual | 15634.999 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `f140e20a17b8e88deca58623218576101ca6b5583405de9b14edfd9985cdba9b`. Saved source ID: `1dfa54330ae1c80249d83108817ccdda2e9a208102ba1cc0d3903af59b2706bc`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
