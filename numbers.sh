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

uv run --frozen --no-sync python -B - "$OUT" "$PROJECT_DIR/NUMBERS.md" <<'EOF'
import json, statistics as st, sys
from datetime import datetime
from pathlib import Path

out, target = Path(sys.argv[1]), Path(sys.argv[2])
runs = json.loads((out / "replay.json").read_text())["runs"]
samples, blocks = {}, {}
for run in runs:
    samples.setdefault(run["engine"], []).extend(run["runtime_ns"])
    blocks.setdefault(run["block"], {})[run["engine"]] = run["process_median_ns"]

ms = lambda ns: ns / 1e6
def mad(values):
    mid = st.median(values)
    return st.median(abs(v - mid) for v in values)

cur, floor, base = (st.median(samples[e]) for e in ("current", "floor", "baseline"))
engine_only = [ms(b["current"] - b["floor"]) for b in blocks.values()]
q1, q2, q3 = st.quantiles(engine_only, n=4)

def latency(log):
    rows = {}
    for line in (out / log).read_text().splitlines():
        head, _, rest = line.partition(": ")
        if head == "median_latency_ns":
            key = "overall"
        elif head.startswith("latency_") and "=" in rest:
            key = head.removeprefix("latency_")
        else:
            continue
        rows[key] = {k.removesuffix("_ns"): v.removesuffix(".0") for k, v in
                     (pair.split("=", 1) for pair in rest.split(",") if "=" in pair)}
    return rows

lines = [f"# Numbers, mixed 1M ({datetime.now():%Y-%m-%d %H:%M})", "",
    f"Raw data: `{out.relative_to(target.parent)}/`. "
    f"{len(blocks)} blocks x {len(runs[0]['runtime_ns'])} reps, binaries in shuffled order per block.", "",
    "## Whole replay", "",
    "| Binary | Median ms | MAD ms | Min ms | Max ms | ns/op |", "| --- | ---: | ---: | ---: | ---: | ---: |"]
for name in ("current", "floor", "baseline"):
    v = samples[name]
    lines.append(f"| {name} | {ms(st.median(v)):.2f} | {ms(mad(v)):.2f} | {ms(min(v)):.2f} | "
                 f"{ms(max(v)):.2f} | {st.median(v) / 1e6:.1f} |")
lines += ["", "## Floor calculation", "",
    "Floor = same harness linked against a stub engine that echoes each request (no matching, no fills).", "",
    f"- Floor: {ms(floor):.2f} ms / 1,000,000 ops = {floor / 1e6:.1f} ns/op",
    f"- Engine only: current - floor per block, median {st.median(engine_only):.2f} ms "
    f"(IQR {q1:.2f} to {q3:.2f}) = {st.median(engine_only):.1f} ns/op",
    f"- Floor share of replay: {ms(floor):.2f} / {ms(cur):.2f} = {100 * floor / cur:.1f}%",
    f"- Speedup vs baseline C++: {ms(base):.2f} / {ms(cur):.2f} = {base / cur:.2f}x",
    "- Subtraction assumes harness and engine costs add; out-of-order overlap makes engine-only an estimate.", "",
    "## Per-operation latency", "",
    "Clock ticks every 41.67 ns on Apple Silicon; values are whole ticks. Two clock reads add ~19 ns/op.", "",
    "| Engine | Kind | Count | p50 ns | p95 ns | p99 ns | p99.9 ns | max ns |",
    "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |"]
for engine in ("current", "baseline"):
    for kind, v in latency(f"latency-{engine}.log").items():
        lines.append(f"| {engine} | {kind} | {v.get('count', '1000000')} | " +
                     " | ".join(v.get(s, "n/a") for s in ("p50", "p95", "p99", "p999", "max")) + " |")
target.write_text("\n".join(lines) + "\n")
print(f"wrote {target}")
EOF
