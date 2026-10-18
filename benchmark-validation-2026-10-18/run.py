"""Run unchanged baseline checks and custom correctness diagnostics, returning failure on mismatch."""

from __future__ import annotations

import hashlib
import json
import shlex
import subprocess
from pathlib import Path

OUTPUT: Path = Path(__file__).resolve().parent
CUSTOM: Path = OUTPUT.parent
BASE: Path = CUSTOM.parent / "polymarket-base"
RUNS: list[dict[str, object]] = []


def run(name: str, command: list[str]) -> int:
    """Capture a command, exit code and full combined output."""
    completed: subprocess.CompletedProcess[str] = subprocess.run(
        command, cwd=BASE, capture_output=True, text=True, check=False
    )
    (OUTPUT / f"{name}.log").write_text(completed.stdout + completed.stderr)
    RUNS.append({"name": name, "command": shlex.join(command), "cwd": str(BASE),
                 "exit_code": completed.returncode})
    (OUTPUT / "commands.json").write_text(json.dumps(RUNS, indent=2) + "\n")
    print(f"{name}: exit={completed.returncode}", flush=True)
    if completed.returncode != 0:
        print(completed.stdout + completed.stderr, end="", flush=True)
    return completed.returncode


def hashes() -> dict[str, str]:
    """Hash engine sources, harness, tests and frozen fixtures."""
    paths: list[Path] = sorted(
        list((CUSTOM / "src").glob("*"))
        + list((BASE / "cpp" / "src").glob("*"))
        + list((BASE / "cpp" / "tests").glob("*"))
        + list((BASE / "polymarkets").glob("*.py"))
        + list((BASE / "tests").glob("*.py"))
        + list((BASE / "traces").glob("*"))
        + list((BASE / "large-traces").glob("*"))
    )
    return {str(path): hashlib.sha256(path.read_bytes()).hexdigest()
            for path in paths if path.is_file()}


def main() -> int:
    """Build diagnostic binaries and validate correctness before any comparison."""
    before: dict[str, str] = hashes()
    (OUTPUT / "summary.json").unlink(missing_ok=True)
    (OUTPUT / "source-hashes.json").write_text(json.dumps(before, indent=2) + "\n")
    flags: list[str] = ["clang++", "-std=c++20", "-O3", "-DNDEBUG"]
    includes: list[str] = [f"-I{BASE / 'cpp/src'}",
        f"-I{BASE / 'cpp/build/_deps/nlohmann_json-src/include'}",
        "-I/opt/homebrew/opt/openssl@3/include"]
    crypto: list[str] = ["-L/opt/homebrew/opt/openssl@3/lib", "-lcrypto"]
    if run("build-custom-check", flags + ["-fno-access-control",
           f"-I{CUSTOM / 'include'}", f"-I{CUSTOM / 'src'}"] + includes +
           [str(OUTPUT / "check.cpp"), str(CUSTOM / "src/orderBook.cpp"),
            str(BASE / "cpp/src/engine.cpp"), str(BASE / "cpp/src/trace.cpp")]
           + crypto + ["-o", str(OUTPUT / "check")]) != 0:
        return 2
    if run("build-baseline-test", flags + includes + [str(BASE / "cpp/tests/engine_test.cpp"),
           str(BASE / "cpp/src/engine.cpp"), "-o", str(OUTPUT / "baseline-test")]) != 0:
        return 2
    if run("build-baseline-benchmark", flags + includes +
           [str(BASE / "cpp/src/benchmark.cpp"), str(BASE / "cpp/src/engine.cpp"),
            str(BASE / "cpp/src/trace.cpp")] + crypto +
           ["-o", str(OUTPUT / "baseline-benchmark")]) != 0:
        return 2
    run("environment", ["uv", "run", "--frozen", "--no-sync", "python", "-B", "-c",
        "import platform; print(platform.platform()); print(platform.python_version())"])
    run("compiler", ["clang++", "--version"])
    run("hardware", ["sysctl", "-n", "machdep.cpu.brand_string"])
    baseline_failures: int = 0
    baseline_failures += run("python-tests", ["uv", "run", "--frozen", "--no-sync",
                             "python", "-B", "-m", "unittest", "discover", "-s", "tests", "-v"]) != 0
    baseline_failures += run("cpp-tests", [str(OUTPUT / "baseline-test")]) != 0
    baseline_failures += run("baseline-parity", ["uv", "run", "--frozen", "--no-sync",
        "python", "-B", "cpp/tests/verify.py", "--binary", str(OUTPUT / "baseline-benchmark"), "--large"]) != 0
    custom_failures: int = int(run("minimal-reproductions", [str(OUTPUT / "check")]) != 0)
    traces: list[Path] = sorted((BASE / "traces").glob("*.jsonl")) + sorted(
        (BASE / "large-traces").glob("*.jsonl"))
    for trace in traces:
        custom_failures += run(trace.stem, [str(OUTPUT / "check"), str(trace)]) != 0
    unchanged: bool = hashes() == before
    summary: dict[str, object] = {"baseline_failed_checks": baseline_failures,
        "custom_failed_checks": custom_failures, "sources_and_fixtures_unchanged": unchanged,
        "performance_comparison": "not run; full fill/result contract requires a compatible API",
        "timed_comparison_batches": 0}
    (OUTPUT / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    print(json.dumps(summary), flush=True)
    return int(baseline_failures != 0 or custom_failures != 0 or not unchanged)


if __name__ == "__main__":
    raise SystemExit(main())
