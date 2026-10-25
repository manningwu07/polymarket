#!/usr/bin/env bash
set -euo pipefail

declare -r project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
exec uv run --project "${project_dir}/../polymarket-base" --frozen --no-sync \
    python -B "${project_dir}/benchmark-native-2026-10-19/compare_profile.py" "$@"
