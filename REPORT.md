# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.391 | 1.110 | 64.8% faster | 15.816 |
| heavy-matching | 10,000 | 0.491 | 1.161 | 57.7% faster | 26.917 |
| mixed | 10,000 | 0.450 | 1.222 | 63.2% faster | 21.368 |
| modification-heavy | 10,000 | 0.494 | 1.467 | 66.3% faster | 25.048 |
| mostly-resting | 10,000 | 0.423 | 1.182 | 64.2% faster | 21.832 |
| mixed | 100,000 | 4.685 | 14.383 | 67.4% faster | 253.008 |
| mixed | 1,000,000 | 52.117 | 214.864 | 75.7% faster | 3308.056 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-11-16T20:41:29.269587+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.391 | 0.007 | 0.375 to 0.440 | 0.0383 | 25,561,849 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.110 | 0.027 | 1.052 to 1.267 | 0.0482 | 9,010,365 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 15.816 | 0.166 | 15.342 to 17.422 | 0.0384 | 632,276 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.491 | 0.008 | 0.462 to 0.513 | 0.0267 | 20,376,974 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.161 | 0.018 | 1.105 to 1.230 | 0.0303 | 8,615,736 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 26.917 | 0.466 | 26.144 to 29.901 | 0.0447 | 371,508 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.450 | 0.005 | 0.435 to 0.467 | 0.0202 | 22,228,397 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.222 | 0.026 | 1.176 to 1.289 | 0.0275 | 8,184,706 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 21.368 | 0.501 | 20.531 to 23.627 | 0.0472 | 467,986 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.494 | 0.012 | 0.478 to 0.521 | 0.0261 | 20,232,676 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.467 | 0.050 | 1.381 to 1.535 | 0.0340 | 6,816,437 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 25.048 | 0.388 | 24.384 to 26.720 | 0.0302 | 399,241 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.423 | 0.004 | 0.399 to 0.435 | 0.0239 | 23,633,678 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.182 | 0.032 | 1.104 to 1.253 | 0.0341 | 8,460,538 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.832 | 0.770 | 20.969 to 23.602 | 0.0430 | 458,049 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 4.685 | 0.039 | 4.573 to 6.925 | 0.1166 | 21,343,200 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.383 | 0.273 | 13.848 to 14.841 | 0.0222 | 6,952,431 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 253.008 | 3.035 | 245.344 to 285.642 | 0.0468 | 395,244 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 52.117 | 0.394 | 51.377 to 56.187 | 0.0205 | 19,187,475 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 214.864 | 5.083 | 207.100 to 256.953 | 0.0675 | 4,654,100 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3308.056 | 79.778 | 2980.600 to 3574.118 | 0.0567 | 302,292 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18051.957 | no |
| warmup | 17040.422 | no |
| setup | 42.849 | no |
| replay | 59412.163 | yes |
| verify | 43083.885 | no |
| disposal | 2217.004 | no |
| reporting | 3249.823 | no |
| measurement stage wall | 158898.546 | no |
| orchestration residual | 15800.444 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `ede239a3625dae597225a32d2867d9df69bbb7cdc7f30bed57b20f8719a6b9fc`. Saved source ID: `8733e5eff029a1ce7faf58c1fb4d13d4ebc220c007206343b19ed6cee5816aa7`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
