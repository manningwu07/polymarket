# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 1.062 | 1.114 | 4.7% faster | 15.821 |
| heavy-matching | 10,000 | 1.433 | 1.173 | 22.1% slower | 26.911 |
| mixed | 10,000 | 1.219 | 1.237 | 1.5% faster | 21.292 |
| modification-heavy | 10,000 | 0.977 | 1.431 | 31.7% faster | 25.693 |
| mostly-resting | 10,000 | 1.322 | 1.222 | 8.2% slower | 21.432 |
| mixed | 100,000 | 15.183 | 14.898 | 1.9% slower | 257.760 |
| mixed | 1,000,000 | 237.924 | 229.348 | 3.7% slower | 3330.901 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-10-25T21:20:16.593184+00:00.
Correctness passed. Build cache: 9/9 commands reused. Correctness cache: 32/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

Environment warnings:

- load average changed from [2.9580078125, 3.70654296875, 3.77880859375] to [4.21923828125, 3.88720703125, 3.837890625]

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 1.062 | 0.014 | 1.019 to 1.164 | 0.0318 | 9,416,568 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.114 | 0.009 | 1.067 to 1.169 | 0.0266 | 8,975,315 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 15.821 | 0.126 | 15.147 to 17.492 | 0.0432 | 632,061 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 1.433 | 0.030 | 1.365 to 1.700 | 0.0595 | 6,980,393 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.173 | 0.020 | 1.142 to 1.221 | 0.0222 | 8,524,851 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 26.911 | 0.230 | 26.200 to 30.455 | 0.0502 | 371,600 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 1.219 | 0.037 | 1.142 to 1.291 | 0.0372 | 8,206,536 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.237 | 0.029 | 1.194 to 1.479 | 0.0550 | 8,083,532 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 21.292 | 0.564 | 20.658 to 23.930 | 0.0561 | 469,661 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.977 | 0.019 | 0.944 to 1.037 | 0.0263 | 10,238,915 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.431 | 0.023 | 1.369 to 1.597 | 0.0367 | 6,989,952 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 25.693 | 1.030 | 24.456 to 27.358 | 0.0400 | 389,215 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 1.322 | 0.022 | 1.285 to 1.457 | 0.0334 | 7,565,252 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.222 | 0.016 | 1.178 to 1.281 | 0.0225 | 8,181,913 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.432 | 0.415 | 21.011 to 23.748 | 0.0498 | 466,582 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 15.183 | 0.239 | 14.412 to 20.477 | 0.0887 | 6,586,278 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.898 | 0.079 | 14.543 to 15.166 | 0.0093 | 6,712,517 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 257.760 | 6.569 | 247.102 to 272.529 | 0.0320 | 387,958 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 237.924 | 9.837 | 227.909 to 271.664 | 0.0585 | 4,203,016 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 229.348 | 10.549 | 218.480 to 323.138 | 0.1150 | 4,360,188 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3330.901 | 74.826 | 2995.651 to 3723.383 | 0.0626 | 300,219 | 15 | 3 | PASS |

Observed custom and baseline ranges overlap for `cancellation-heavy-10000-seed42`, `mixed-10000-seed42`, `mixed-100000-seed42`, `mixed-1000000-seed42`. This sample set does not establish a winner for those workloads.

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 19461.866 | no |
| warmup | 17827.636 | no |
| setup | 44.547 | no |
| replay | 63401.397 | yes |
| verify | 44710.112 | no |
| disposal | 2523.325 | no |
| reporting | 3269.422 | no |
| measurement stage wall | 166898.511 | no |
| orchestration residual | 15660.207 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `70a976f3073c847b7f0ff9fb16dda7378ccaf579262bbdf69e7a86fdd2b09fae`. Saved source ID: `1648f5f2a0e5820c3b227c3801f9a2040508925bf76561487d82c3affb734c20`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
