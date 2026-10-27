# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.399 | 1.089 | 63.4% faster | 16.185 |
| heavy-matching | 10,000 | 0.548 | 1.155 | 52.6% faster | 27.409 |
| mixed | 10,000 | 0.463 | 1.233 | 62.5% faster | 21.274 |
| modification-heavy | 10,000 | 0.442 | 1.408 | 68.6% faster | 26.726 |
| mostly-resting | 10,000 | 0.428 | 1.157 | 63.0% faster | 21.698 |
| mixed | 100,000 | 5.647 | 14.199 | 60.2% faster | 259.057 |
| mixed | 1,000,000 | 76.357 | 228.987 | 66.7% faster | 3388.016 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-10-27T14:39:37.584169+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.399 | 0.006 | 0.392 to 0.418 | 0.0232 | 25,083,591 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.089 | 0.024 | 1.045 to 1.235 | 0.0469 | 9,179,575 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 16.185 | 0.208 | 15.570 to 17.940 | 0.0447 | 617,845 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.548 | 0.019 | 0.522 to 0.621 | 0.0510 | 18,263,473 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.155 | 0.017 | 1.079 to 1.242 | 0.0392 | 8,660,821 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 27.409 | 0.191 | 26.703 to 30.341 | 0.0423 | 364,845 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.463 | 0.007 | 0.452 to 0.582 | 0.0741 | 21,619,705 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.233 | 0.033 | 1.173 to 1.275 | 0.0303 | 8,108,386 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 21.274 | 0.443 | 20.781 to 23.521 | 0.0489 | 470,050 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.442 | 0.005 | 0.435 to 0.540 | 0.0725 | 22,603,110 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.408 | 0.023 | 1.345 to 1.564 | 0.0344 | 7,101,642 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 26.726 | 0.327 | 24.675 to 30.020 | 0.0496 | 374,170 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.428 | 0.011 | 0.410 to 0.488 | 0.0553 | 23,346,322 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.157 | 0.015 | 1.103 to 1.203 | 0.0212 | 8,639,309 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 21.698 | 0.103 | 21.387 to 23.866 | 0.0433 | 460,867 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 5.647 | 0.061 | 5.586 to 8.270 | 0.1110 | 17,709,302 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.199 | 0.216 | 13.888 to 14.532 | 0.0154 | 7,042,749 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 259.057 | 5.974 | 251.961 to 274.868 | 0.0280 | 386,015 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 76.357 | 1.145 | 73.565 to 94.563 | 0.0642 | 13,096,348 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 228.987 | 10.679 | 210.140 to 262.981 | 0.0678 | 4,367,051 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3388.016 | 148.158 | 3012.518 to 3774.662 | 0.0593 | 295,158 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18291.719 | no |
| warmup | 17828.709 | no |
| setup | 71.787 | no |
| replay | 62099.515 | yes |
| verify | 44918.055 | no |
| disposal | 2250.550 | no |
| reporting | 3329.942 | no |
| measurement stage wall | 164856.989 | no |
| orchestration residual | 16066.712 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `44a103e608cbbbd7f8fa64c10d36802c54f23ba0a4025d3833fc34a0af649f1e`. Saved source ID: `0f618a23b69876a7a80aac3d286a3d70ef62e3472ac0bb72ca18ea7e19409d50`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
