<!--Profile args for runs-->
--trace /Users/manningwu/Desktop/personal-projects/polymarket-base/large-traces/mixed-1000000-seed42.jsonl --expected /Users/manningwu/Desktop/personal-projects/polymarket-base/large-traces/mixed-1000000-seed42.expected.json --repetitions 5

<!--Run cmds-->
uv run --frozen --no-sync python -B ../polymarket/benchmark-native-2026-10-19/run.py
uv run --frozen --no-sync python -B ../polymarket/benchmark-native-2026-10-19/measure.py
<!--E2E testing-->
./benchmark-native-2026-10-19/benchmark.sh