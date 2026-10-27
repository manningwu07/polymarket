# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.612 | 1.134 | 46.0% faster | 15.515 |
| heavy-matching | 10,000 | 0.861 | 1.138 | 24.3% faster | 26.412 |
| mixed | 10,000 | 0.666 | 1.159 | 42.6% faster | 20.685 |
| modification-heavy | 10,000 | 0.587 | 1.380 | 57.5% faster | 24.361 |
| mostly-resting | 10,000 | 0.696 | 1.161 | 40.0% faster | 21.457 |
| mixed | 100,000 | 8.245 | 13.762 | 40.1% faster | 252.780 |
| mixed | 1,000,000 | 147.015 | 212.105 | 30.7% faster | 3294.957 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-10-27T00:56:25.893380+00:00.
Correctness passed. Build cache: 9/9 commands reused. Correctness cache: 32/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

Environment warnings:

- load average changed from [5.4228515625, 3.3212890625, 2.80908203125] to [2.689453125, 2.9814453125, 2.75244140625]

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.612 | 0.008 | 0.591 to 0.661 | 0.0323 | 16,328,770 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.134 | 0.026 | 1.099 to 1.388 | 0.0777 | 8,815,101 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 15.515 | 0.160 | 15.266 to 17.060 | 0.0385 | 644,525 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.861 | 0.043 | 0.786 to 0.914 | 0.0523 | 11,608,779 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.138 | 0.012 | 1.113 to 1.191 | 0.0188 | 8,785,416 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 26.412 | 0.426 | 25.758 to 29.695 | 0.0461 | 378,611 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.666 | 0.013 | 0.647 to 0.703 | 0.0262 | 15,016,909 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.159 | 0.016 | 1.132 to 1.206 | 0.0173 | 8,624,407 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 20.685 | 0.273 | 20.385 to 22.965 | 0.0492 | 483,433 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.587 | 0.007 | 0.571 to 0.617 | 0.0199 | 17,038,184 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.380 | 0.025 | 1.339 to 1.410 | 0.0176 | 7,245,936 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 24.361 | 0.711 | 23.450 to 25.978 | 0.0352 | 410,496 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.696 | 0.026 | 0.661 to 0.789 | 0.0518 | 14,360,945 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.161 | 0.033 | 1.093 to 1.282 | 0.0452 | 8,615,431 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.457 | 0.269 | 20.838 to 24.107 | 0.0458 | 466,058 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 8.245 | 0.051 | 8.073 to 8.366 | 0.0100 | 12,128,869 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 13.762 | 0.094 | 13.541 to 14.084 | 0.0105 | 7,266,364 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 252.780 | 5.075 | 244.147 to 278.255 | 0.0434 | 395,600 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 147.015 | 0.804 | 145.444 to 163.079 | 0.0278 | 6,802,014 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 212.105 | 2.280 | 209.234 to 223.101 | 0.0197 | 4,714,657 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3294.957 | 129.332 | 2935.503 to 3607.614 | 0.0577 | 303,494 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18082.384 | no |
| warmup | 17254.244 | no |
| setup | 42.409 | no |
| replay | 60654.855 | yes |
| verify | 43157.123 | no |
| disposal | 2348.804 | no |
| reporting | 3277.900 | no |
| measurement stage wall | 160491.586 | no |
| orchestration residual | 15673.867 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `f1c85da5f2b00c6fa3af8c5833ca401f647409b379f70397b26dd3503ccc7ea6`. Saved source ID: `9ac8637133b764c378ed012fd8180fd35f578058b0f88d681382cb9be53bd1d4`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
