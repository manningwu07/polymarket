# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.459 | 1.196 | 61.6% faster | 16.843 |
| heavy-matching | 10,000 | 0.555 | 1.231 | 54.9% faster | 28.075 |
| mixed | 10,000 | 0.505 | 1.266 | 60.1% faster | 22.211 |
| modification-heavy | 10,000 | 0.554 | 1.514 | 63.4% faster | 26.164 |
| mostly-resting | 10,000 | 0.457 | 1.206 | 62.1% faster | 22.477 |
| mixed | 100,000 | 5.366 | 14.821 | 63.8% faster | 270.462 |
| mixed | 1,000,000 | 62.182 | 218.623 | 71.6% faster | 3434.327 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-11-11T20:51:46.222073+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 2/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.459 | 0.007 | 0.438 to 0.469 | 0.0186 | 21,802,357 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.196 | 0.029 | 1.148 to 1.277 | 0.0314 | 8,361,498 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 16.843 | 0.076 | 15.926 to 19.156 | 0.0515 | 593,710 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.555 | 0.011 | 0.523 to 0.777 | 0.1034 | 18,013,961 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.231 | 0.053 | 1.126 to 1.404 | 0.0705 | 8,125,675 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 28.075 | 0.756 | 27.224 to 32.392 | 0.0566 | 356,188 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.505 | 0.005 | 0.497 to 0.535 | 0.0234 | 19,787,287 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.266 | 0.023 | 1.218 to 1.310 | 0.0228 | 7,899,156 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 22.211 | 0.462 | 21.702 to 24.488 | 0.0481 | 450,233 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.554 | 0.003 | 0.548 to 0.578 | 0.0132 | 18,050,542 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.514 | 0.036 | 1.457 to 1.598 | 0.0269 | 6,604,475 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 26.164 | 0.477 | 25.671 to 28.584 | 0.0376 | 382,204 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.457 | 0.003 | 0.444 to 0.525 | 0.0403 | 21,897,794 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.206 | 0.015 | 1.157 to 1.276 | 0.0267 | 8,293,015 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 22.477 | 0.623 | 21.841 to 24.880 | 0.0476 | 444,894 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 5.366 | 0.129 | 5.181 to 5.852 | 0.0337 | 18,637,158 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.821 | 0.076 | 14.371 to 15.043 | 0.0109 | 6,747,259 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 270.462 | 6.864 | 259.644 to 286.674 | 0.0309 | 369,738 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 62.182 | 3.697 | 58.484 to 75.484 | 0.0987 | 16,081,835 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 218.623 | 3.214 | 214.206 to 300.571 | 0.0950 | 4,574,075 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3434.327 | 82.290 | 3137.370 to 3719.350 | 0.0481 | 291,178 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18661.829 | no |
| warmup | 18288.496 | no |
| setup | 44.407 | no |
| replay | 62439.131 | yes |
| verify | 45419.009 | no |
| disposal | 2215.745 | no |
| reporting | 3393.782 | no |
| measurement stage wall | 166970.331 | no |
| orchestration residual | 16507.932 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `e162ac22e8ef0ef139f74d5e1e1d05f95f89c491974a8cc6705aa00008627cf5`. Saved source ID: `96d34babe303174f0faf299370a9235245f43b20a1c7b7df41aa817fc6e4d90b`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
