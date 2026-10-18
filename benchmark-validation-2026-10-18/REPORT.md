# Correctness gate, 2026-10-18

The custom engine fails the frozen contract. No comparable performance measurements or speedups were produced. Both original baselines pass their existing checks. Engine sources, tests, harness and trace bytes remain unchanged, verified by before/after SHA-256 maps. The baseline directory remains immutable.

| Implementation | Fresh validation | Performance comparison |
| --- | --- | --- |
| Python baseline | 68 existing tests pass | Withheld |
| C++ baseline | Existing engine tests and portable parity verifier pass, including 1M | Withheld |
| Custom C++ | Four minimal cases fail; all seven shipped traces fail the diagnostic gate | Ineligible |

## Reproduced mismatches

1. The user corrected `src/orderHeap.tpp:86` to choose the higher-priority child, `higher_priority_(heap_[l], heap_[r]) ? l : r`. The ask/bid market-priority reproductions and direct heap-pop check now pass. Previously, asks at 100, 101, 102 and 103 left root 102 above child 101 after popping 100, causing a market buy for two to fill 100 and 102.
2. `submit_market` does not consume an exchange sequence. Submit a limit, a market, then another limit. The last limit gets sequence 2 rather than 3. Every accepted submission and modification must consume one sequence, including empty-book markets. Cancellation and rejection consume none.
3. Duplicate active IDs are accepted. Two submissions with ID 1 produce two heap entries but one index entry. Reject duplicate submissions before mutation. Submitted IDs remain retired for the book lifetime, including canceled, filled and unfilled market IDs. Modification retains its existing ID.
4. Zero quantity is accepted. The frozen contract requires positive prices and quantities and valid sides. Missing cancel/modify targets must raise the contract's missing-order error, rather than merely return false. Rejected operations must preserve state and sequence.

Both mixed traces, 100K and 1M, now fail at zero-based event 1040 with a sequence mismatch, expected 1039 versus actual 1037. The cancellation-heavy fixture first fails at event 1046. The modification-heavy and mostly-resting fixtures first expose sequence mismatches at events 1026 and 1029. Each diagnostic replay stops at its first failure; none completed successfully. See individual logs for exact trace hashes and failures.

## Output mismatch and adapter limits

The baseline requires each submit/modify to return a residual order and ordered fills, and cancellation to return the removed order. Each fill records maker ID, taker ID, maker price and quantity. Order records include side, type, nullable price, residual quantity and sequence. Market remainder appears in the result but never rests. Snapshots return independent order values in price/FIFO order.

The custom API returns void for submissions and bool for cancel/modify. It discards fill history and market residuals, and has no public snapshot. A wrapper can translate string IDs to stable numeric IDs, but cannot recover the missing execution record faithfully without observing matching or duplicating matching logic. To satisfy the contract, capture fills where matches occur, expose the required result values and provide snapshots. This adds required work that the baselines already perform inside timing. Timing the current void API would compare different work. No engine changes were made.

The original correctness suites cannot run unchanged against this incompatible API. The added diagnostic instead uses the original native trace parser and baseline engine, remaps IDs bijectively, and checks heap priority, index cardinality/inverse mapping and remaining orders after each event. `-fno-access-control` permits diagnostic reads of private state without source edits. It is diagnostic-only and supplies no performance evidence. It cannot verify ordered fills because the custom engine exposes none.

## Reproduce and inspect

From `polymarket-base`, run `uv run --frozen --no-sync python -B ../polymarket/benchmark-validation-2026-10-18/run.py`. Each run rebuilds the latest code and prints failures directly. Exit 0 means the implemented diagnostic checks passed, 1 means a check failed, and 2 means compilation failed. Full fill-contract validation still requires a compatible engine API. `commands.json` records every exact compile/run command, working directory and exit code. Individual `.log` files preserve complete outputs; `summary.json` records the gate outcome; `source-hashes.json` identifies the inputs. These outputs are replaced on each run. Builds use Apple Clang 17, C++20, `-O3 -DNDEBUG` on Apple M4 Pro. Python uses the baseline project's existing local venv and uv.

The existing harness already provides per-operation p50/p95/p99, includes clocks and retained operation results in replay timing, and excludes parsing, hashing and correctness collection. It prints aggregate medians, without exporting individual repetition runtimes. The requested three batches of five remain unrun because correctness failed. Allocations, cache locality and operation bottlenecks have not been profiled; no performance explanations or optimization recommendations follow from these correctness diagnostics.
