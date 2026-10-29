# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.517 | 1.127 | 54.1% faster | 16.355 |
| heavy-matching | 10,000 | 0.705 | 1.162 | 39.4% faster | 27.578 |
| mixed | 10,000 | 0.584 | 1.254 | 53.4% faster | 21.696 |
| modification-heavy | 10,000 | 0.510 | 1.490 | 65.8% faster | 25.902 |
| mostly-resting | 10,000 | 0.555 | 1.201 | 53.8% faster | 21.913 |
| mixed | 100,000 | 6.633 | 14.470 | 54.2% faster | 259.073 |
| mixed | 1,000,000 | 78.400 | 226.714 | 65.4% faster | 3452.628 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-10-29T20:00:01.565024+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.517 | 0.005 | 0.504 to 0.537 | 0.0174 | 19,339,255 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.127 | 0.015 | 1.082 to 1.192 | 0.0232 | 8,872,130 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 16.355 | 0.194 | 15.749 to 17.844 | 0.0368 | 611,426 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.705 | 0.014 | 0.654 to 0.846 | 0.0683 | 14,187,758 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.162 | 0.018 | 1.140 to 1.210 | 0.0213 | 8,604,623 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 27.578 | 0.225 | 26.971 to 30.562 | 0.0405 | 362,610 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.584 | 0.013 | 0.565 to 0.643 | 0.0348 | 17,109,869 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.254 | 0.018 | 1.213 to 1.289 | 0.0180 | 7,972,365 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 21.696 | 0.338 | 21.293 to 23.774 | 0.0451 | 460,909 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.510 | 0.009 | 0.499 to 0.569 | 0.0453 | 19,590,250 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.490 | 0.029 | 1.451 to 1.673 | 0.0444 | 6,709,158 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 25.902 | 0.346 | 25.063 to 29.500 | 0.0431 | 386,064 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.555 | 0.005 | 0.546 to 0.579 | 0.0196 | 18,016,655 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.201 | 0.020 | 1.176 to 1.271 | 0.0219 | 8,329,571 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.913 | 0.227 | 21.617 to 24.311 | 0.0477 | 456,351 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 6.633 | 0.046 | 6.394 to 6.702 | 0.0132 | 15,076,703 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.470 | 0.077 | 14.107 to 14.687 | 0.0105 | 6,910,671 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 259.073 | 2.800 | 251.822 to 288.337 | 0.0447 | 385,991 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 78.400 | 1.056 | 76.360 to 85.834 | 0.0285 | 12,755,061 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 226.714 | 3.735 | 221.262 to 254.718 | 0.0396 | 4,410,842 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3452.628 | 123.447 | 3022.671 to 3755.547 | 0.0615 | 289,634 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18569.928 | no |
| warmup | 17741.588 | no |
| setup | 43.258 | no |
| replay | 61820.675 | yes |
| verify | 44984.456 | no |
| disposal | 2180.665 | no |
| reporting | 3329.843 | no |
| measurement stage wall | 164838.763 | no |
| orchestration residual | 16168.349 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `2bc2dc8e124ffda5b2fc2f8a7d2baf7f39461af809cb9b5d7dbdf439bf236779`. Saved source ID: `148a0c567edaeca70693ac7659fbc4be4767f3153a823b841b685ee73f5a72b2`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
