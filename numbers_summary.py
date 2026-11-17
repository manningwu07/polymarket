import json, statistics as st, sys
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

lines = [f"# Numbers, mixed 1M (run started {runs[0]['started_at'][:16].replace('T', ' ')} UTC)", "",
    f"Raw data: `{out.relative_to(target.parent)}/`. "
    f"{len(blocks)} blocks x {len(runs[0]['runtime_ns'])} reps, binaries in shuffled order per block.", "",
    "## Whole replay", "",
    "| Binary | Median ms | MAD ms | Min ms | Max ms | ns/op |", "| --- | ---: | ---: | ---: | ---: | ---: |"]
for name in ("current", "floor", "baseline"):
    v = samples[name]
    lines.append(f"| {name} | {ms(st.median(v)):.2f} | {ms(mad(v)):.2f} | {ms(min(v)):.2f} | "
                 f"{ms(max(v)):.2f} | {st.median(v) / 1e6:.1f} |")
c = samples["current"]
p10, p90 = st.quantiles(c, n=10)[0], st.quantiles(c, n=10)[-1]
outliers = [f"block {r['block'] + 1} rep {i + 1}: {ms(v):.2f}" for r in runs if r["engine"] == "current"
            for i, v in enumerate(r["runtime_ns"]) if v > 1.1 * cur]
order = sorted(blocks)
edge = max(1, len(order) // 4)
lines += ["", "## Distribution and drift (current)", "",
    f"- p10 to p90: {ms(p10):.2f} to {ms(p90):.2f} ms; min {ms(min(c)):.2f}, max {ms(max(c)):.2f}",
    f"- Reps more than 10% above median ({len(outliers)} of {len(c)}): {', '.join(outliers) or 'none'}", "",
    f"Per-block medians, first {edge} vs last {edge} blocks:", "",
    "| Binary | First blocks ms | Last blocks ms | Change |", "| --- | ---: | ---: | ---: |"]
for name in ("current", "floor"):
    first = st.median(blocks[b][name] for b in order[:edge])
    last = st.median(blocks[b][name] for b in order[-edge:])
    lines.append(f"| {name} | {ms(first):.2f} | {ms(last):.2f} | {100 * (last / first - 1):+.1f}% |")
lines += ["", "If the floor stub drifts by the same fraction as the engine, the drift is machine-wide "
    "(likely thermal or clock-frequency change), not engine behaviour. A single slow rep inside a "
    "process with normal neighbours points to outside interference."]
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
