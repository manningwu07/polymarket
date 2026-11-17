#!/usr/bin/env bash
set -euo pipefail

# Measure the headline numbers on mixed 1M and write NUMBERS.md (REPORT.md is untouched).
#   1. Paired whole-replay runs: current engine vs harness floor stub vs baseline C++.
#   2. Per-operation latency (p50..max) for current and baseline.
# Usage: ./numbers.sh [blocks] [repetitions]   defaults: 20 5
# Needs benchmark.sh to have run once (generated runner + baseline-benchmark).
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HARNESS_DIR="$PROJECT_DIR/benchmark-native-2026-10-19"
BASE_DIR="$(cd "$PROJECT_DIR/../polymarket-base" && pwd)"
BLOCKS="${1:-20}"
REPS="${2:-5}"
OUT="$PROJECT_DIR/profiles/numbers-$(date +%Y%m%d-%H%M)"
TRACE="large-traces/mixed-1000000-seed42.jsonl"
BASELINE="$HARNESS_DIR/baseline-benchmark"

[[ -x "$BASELINE" ]] || { echo "missing $BASELINE; run benchmark.sh once first" >&2; exit 1; }
mkdir -p "$OUT"

echo "== build current (working tree src/) and floor stub"
"$PROJECT_DIR/compile.sh" "$PROJECT_DIR/src" "$OUT/current-benchmark"
"$HARNESS_DIR/floor/build.sh"
cp "$PROJECT_DIR/build/floor/floor-benchmark" "$OUT/floor-benchmark"

cd "$BASE_DIR"
echo "== paired whole replay: $BLOCKS blocks x $REPS reps"
uv run --frozen --no-sync python -B "$HARNESS_DIR/ab_paired.py" \
  current="$OUT/current-benchmark" floor="$OUT/floor-benchmark" baseline="$BASELINE" \
  --unverified floor --blocks "$BLOCKS" --repetitions "$REPS" --warmups 1 \
  --output "$OUT/replay.json" | tee "$OUT/replay.log"

for engine in current baseline; do
  echo "== per-op latency: $engine"
  binary="$OUT/current-benchmark"
  [[ "$engine" == baseline ]] && binary="$BASELINE"
  "$binary" --trace "$TRACE" --expected "${TRACE%.jsonl}.expected.json" \
    --repetitions "$REPS" --warmups 1 --latency > "$OUT/latency-$engine.log"
done

uv run --frozen --no-sync python -B "$PROJECT_DIR/numbers_summary.py" "$OUT" "$PROJECT_DIR/NUMBERS.md"
