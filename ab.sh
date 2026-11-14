#!/usr/bin/env bash
set -euo pipefail

# A/B the working tree (after) against a git ref (before, default HEAD) on the 1M mixed trace.
# Usage: ./ab.sh <label> [ref]
# Writes profiles/ab-builds/<YYYYMMDD>-<label>/{before,after,mixed1M.json}; refuses to reuse a folder.
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LABEL="${1:?usage: ./ab.sh <label> [ref]}"
REF="${2:-HEAD}"
OUT="$PROJECT_DIR/profiles/ab-builds/$(date +%Y%m%d)-$LABEL"

if [[ -e "$OUT" ]]; then
  echo "Error: $OUT already exists; pick another label." >&2
  exit 1
fi

REF_SRC="$(mktemp -d)"
trap 'rm -rf "$REF_SRC"' EXIT
git -C "$PROJECT_DIR" archive "$REF" src | tar -x -C "$REF_SRC"

printf '== before: %s\n' "$REF"
"$PROJECT_DIR/compile.sh" "$REF_SRC/src" "$OUT/before"
printf '== after: working tree\n'
"$PROJECT_DIR/compile.sh" "$PROJECT_DIR/src" "$OUT/after"

cd "$PROJECT_DIR/../polymarket-base"
uv run --frozen --no-sync python -B "$PROJECT_DIR/benchmark-native-2026-10-19/ab_paired.py" \
  before="$OUT/before" after="$OUT/after" \
  --blocks 20 --repetitions 5 --warmups 1 --output "$OUT/mixed1M.json"
