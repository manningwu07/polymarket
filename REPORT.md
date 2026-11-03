# Matching-engine benchmark

| Workload | Ops | Custom ms | Baseline C++ ms | Custom runtime change | Python ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| cancellation-heavy | 10,000 | 0.463 | 1.121 | 58.7% faster | 16.191 |
| heavy-matching | 10,000 | 0.567 | 1.220 | 53.5% faster | 27.355 |
| mixed | 10,000 | 0.547 | 1.222 | 55.2% faster | 22.197 |
| modification-heavy | 10,000 | 0.570 | 1.454 | 60.8% faster | 26.757 |
| mostly-resting | 10,000 | 0.467 | 1.203 | 61.1% faster | 22.417 |
| mixed | 100,000 | 5.486 | 14.333 | 61.7% faster | 258.255 |
| mixed | 1,000,000 | 60.568 | 218.013 | 72.2% faster | 3417.036 |

Runtime change is `(custom - baseline) / baseline * 100`. The table describes the retained samples. It does not assign a cause to a difference.

## Run status

The runner measured this set at 2026-11-03T21:17:56.555920+00:00.
Correctness passed. Build cache: 0/9 commands reused. Correctness cache: 0/33 checks reused.

These caches store process results, logs, binaries, and exports. They do not describe the CPU's hardware caches.

## Per-implementation detail

MAD and range describe the retained trials. They are not confidence intervals.

| Workload | Implementation | Median ms | MAD ms | Range ms | CV | Ops/s | Trials | Warmups | Correctness |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| cancellation-heavy-10000-seed42 | custom | 0.463 | 0.003 | 0.449 to 0.471 | 0.0119 | 21,615,780 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | baseline | 1.121 | 0.022 | 1.048 to 1.279 | 0.0493 | 8,922,262 | 15 | 3 | PASS |
| cancellation-heavy-10000-seed42 | python | 16.191 | 0.200 | 15.598 to 17.806 | 0.0397 | 617,633 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | custom | 0.567 | 0.022 | 0.518 to 0.636 | 0.0570 | 17,623,723 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | baseline | 1.220 | 0.066 | 1.139 to 1.421 | 0.0686 | 8,198,684 | 15 | 3 | PASS |
| heavy-matching-10000-seed42 | python | 27.355 | 0.308 | 26.681 to 31.287 | 0.0480 | 365,567 | 15 | 3 | PASS |
| mixed-10000-seed42 | custom | 0.547 | 0.045 | 0.478 to 0.810 | 0.1698 | 18,274,587 | 15 | 3 | PASS |
| mixed-10000-seed42 | baseline | 1.222 | 0.019 | 1.183 to 1.400 | 0.0490 | 8,186,656 | 15 | 3 | PASS |
| mixed-10000-seed42 | python | 22.197 | 0.653 | 20.748 to 25.214 | 0.0580 | 450,512 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | custom | 0.570 | 0.007 | 0.563 to 0.642 | 0.0366 | 17,541,275 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | baseline | 1.454 | 0.018 | 1.392 to 1.521 | 0.0214 | 6,879,353 | 15 | 3 | PASS |
| modification-heavy-10000-seed42 | python | 26.757 | 1.137 | 25.390 to 30.535 | 0.0514 | 373,733 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | custom | 0.467 | 0.007 | 0.437 to 0.704 | 0.1539 | 21,392,250 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | baseline | 1.203 | 0.034 | 1.146 to 1.423 | 0.0552 | 8,313,989 | 15 | 3 | PASS |
| mostly-resting-10000-seed42 | python | 22.417 | 0.277 | 22.140 to 25.084 | 0.0487 | 446,088 | 15 | 3 | PASS |
| mixed-100000-seed42 | custom | 5.486 | 0.063 | 5.308 to 6.919 | 0.0768 | 18,229,463 | 15 | 3 | PASS |
| mixed-100000-seed42 | baseline | 14.333 | 0.200 | 14.061 to 15.339 | 0.0259 | 6,976,947 | 15 | 3 | PASS |
| mixed-100000-seed42 | python | 258.255 | 7.796 | 248.052 to 307.393 | 0.0561 | 387,214 | 15 | 3 | PASS |
| mixed-1000000-seed42 | custom | 60.568 | 0.618 | 59.571 to 78.888 | 0.0753 | 16,510,380 | 15 | 3 | PASS |
| mixed-1000000-seed42 | baseline | 218.013 | 3.764 | 211.338 to 245.999 | 0.0464 | 4,586,875 | 15 | 3 | PASS |
| mixed-1000000-seed42 | python | 3417.036 | 152.803 | 3022.983 to 3741.304 | 0.0619 | 292,651 | 15 | 3 | PASS |

## Timing contract

Primary results use `polymarkets-whole-replay-v2`. Each trial starts with a fresh book and preallocated output storage. The replay clock covers the full operation loop. Engine calls include their validation, matching, mutations, allocations, frees, and copies. The loop also retains each native output. It has no per-event clocks.

Parsing, setup, the untimed audit, warmups, result normalization, verification, retained-result disposal, artifact writes, and reporting stay outside the replay clock. `phase-timings.json` records their totals separately.

| Phase | Total ms | Reported as engine performance |
| --- | ---: | --- |
| load | 18225.392 | no |
| warmup | 17382.907 | no |
| setup | 43.922 | no |
| replay | 61264.415 | yes |
| verify | 44533.502 | no |
| disposal | 2188.035 | no |
| reporting | 3302.199 | no |
| measurement stage wall | 162895.078 | no |
| orchestration residual | 15954.706 | no |

The runner-phase rows sum work across child processes. Measurement stage wall is the coarse parent-process scope before artifact save and report rendering. The residual includes process startup, hashing, compression, artifact I/O, and orchestration. Do not sum the coarse scope rows with the runner-phase rows.

## Evidence

Timing contract: `polymarkets-whole-replay-v2`. Benchmark contract: `polymarkets-benchmark-v1`. Custom binary SHA-256: `04f73d2fce2c362c7bdd7315113bc78ba13086d094cbf4646fcd1714f4fd5f2a`. Saved source ID: `3dd3872635d0d086c7050f887f71ca31ee1f26f73b3cc2e47e44a62ab2aa87ea`.

`samples/` holds every runner sidecar. `measurements.json` retains every replay and warmup sample. `aggregates.json` contains statistics checked against those samples. `exports/` holds the compressed portable results that matched the frozen expected documents. `source-hashes.json`, `build-metadata.json`, and `artifact-hashes.json` bind the inputs, compilers, commands, and proof artifacts.

Per-operation latency was not run. Use `measure.py --latency` for a separate `polymarkets-legacy-instrumented-v1` diagnostic set.

See [README.md](README.md) for commands and [AUDIT.md](AUDIT.md) for timing and cache history.
