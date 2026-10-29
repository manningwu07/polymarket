# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.482 | 1.169 | 58.8% faster | 17.289 |
| heavy-matching | 10,000 | 0.661 | 1.192 | 44.5% faster | 29.407 |
| mixed | 10,000 | 0.565 | 1.229 | 54.0% faster | 22.231 |
| modification-heavy | 10,000 | 0.515 | 1.502 | 65.7% faster | 27.308 |
| mostly-resting | 10,000 | 0.503 | 1.229 | 59.1% faster | 22.465 |
| mixed | 100,000 | 6.448 | 14.929 | 56.8% faster | 270.944 |
| mixed | 1,000,000 | 74.523 | 290.128 | 74.3% faster | 3463.209 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-10-29T14:37:10.196869+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

Environment warnings:

- load average changed from [2.7470703125, 3.5244140625, 3.408203125] to [3.86083984375, 3.74951171875, 3.51806640625]

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.482 | 0.016 | 0.450 to 0.862 | 0.2275 | 20,752,270 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.169 | 0.051 | 1.097 to 1.582 | 0.0973 | 8,550,663 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 17.289 | 0.444 | 16.758 to 20.222 | 0.0566 | 578,401 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.661 | 0.017 | 0.621 to 0.788 | 0.0652 | 15,132,416 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.192 | 0.030 | 1.133 to 1.228 | 0.0252 | 8,392,782 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 29.407 | 1.010 | 27.906 to 32.568 | 0.0490 | 340,059 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.565 | 0.045 | 0.512 to 1.342 | 0.3171 | 17,703,032 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.229 | 0.019 | 1.202 to 1.465 | 0.0633 | 8,136,697 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 22.231 | 0.390 | 21.777 to 25.649 | 0.0619 | 449,827 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.515 | 0.035 | 0.473 to 0.794 | 0.1617 | 19,400,223 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.502 | 0.030 | 1.432 to 1.783 | 0.0592 | 6,655,760 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 27.308 | 1.094 | 26.027 to 30.987 | 0.0495 | 366,196 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.503 | 0.005 | 0.466 to 0.867 | 0.1768 | 19,888,940 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.229 | 0.057 | 1.169 to 1.615 | 0.0914 | 8,136,418 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 22.465 | 0.501 | 21.965 to 25.703 | 0.0556 | 445,129 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 6.448 | 0.198 | 6.248 to 8.642 | 0.0898 | 15,507,581 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.929 | 0.273 | 14.308 to 16.550 | 0.0364 | 6,698,241 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 270.944 | 8.985 | 255.930 to 294.544 | 0.0388 | 369,080 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 74.523 | 0.615 | 73.580 to 80.393 | 0.0243 | 13,418,759 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 290.128 | 36.747 | 218.030 to 327.753 | 0.1613 | 3,446,750 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3463.209 | 104.704 | 3057.584 to 3754.903 | 0.0586 | 288,749 | 15 | 3 | PASS |

Observed custom and baseline ranges overlap for `mixed-10000-seed42`. This sample set does not establish a winner for those workloads.

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18733.053 | no |
| warmup | 18269.176 | no |
| setup | 73.095 | no |
| replay | 63281.218 | yes |
| verify | 45792.713 | no |
| disposal | 2324.695 | no |
| reporting | 3345.182 | no |
| measurement stage wall | 168197.559 | no |
| orchestration residual | 16378.427 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `4dcc7b057ef7aac37e1b48700099c96b45500fad0f35a3abc6f859535170ece2`. Saved source ID: `9e8606647b2ab84ac57df1edfbf2b4516df3d6128646a932f24070bbfec194b0`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
