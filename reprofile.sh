#!/usr/bin/env bash
set -euo pipefail

# Rebuild the native Polymarkets benchmark for source-level Instruments profiling.
# Put this script in polymarket/reprofile.sh; run it from any directory.
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(cd "$PROJECT_DIR/../polymarket-base" && pwd)"
PROFILE_DIR="$PROJECT_DIR/profiles"
NATIVE_DIR="$PROJECT_DIR/benchmark-native-2026-10-19/native"

if ! command -v brew >/dev/null 2>&1; then
  echo "Error: Homebrew is required to locate OpenSSL 3." >&2
  exit 1
fi
OPENSSL_PREFIX="$(brew --prefix openssl@3)"
JSON_INCLUDE="$BASE_DIR/cpp/build/_deps/nlohmann_json-src/include"

for file in \
  "$NATIVE_DIR/benchmark.cpp" \
  "$NATIVE_DIR/trace.cpp" \
  "$PROJECT_DIR/src/orderBook.cpp" \
  "$JSON_INCLUDE/nlohmann/json.hpp" \
  "$OPENSSL_PREFIX/include/openssl/evp.h"; do
  if [[ ! -f "$file" ]]; then
    echo "Missing required file: $file" >&2
    exit 1
  fi
done

# Version builds so existing traces can retain their matching binaries/dSYMs.
BUILD_ID="$(date +%Y%m%d-%H%M%S)-$$"
OBJ_DIR="$PROFILE_DIR/obj/$BUILD_ID"
BIN="$PROFILE_DIR/custom-benchmark-$BUILD_ID"
mkdir -p "$OBJ_DIR"

CXXFLAGS=(
  -std=c++20 -O3 -DNDEBUG -g -fno-omit-frame-pointer
  -I "$BASE_DIR/cpp/src"
  -I "$JSON_INCLUDE"
  -I "$PROJECT_DIR/src"
  -I "$OPENSSL_PREFIX/include"
)

cd "$BASE_DIR"
for name in benchmark trace; do
  echo "Compiling $name.cpp..."
  xcrun clang++ "${CXXFLAGS[@]}" -c "$NATIVE_DIR/$name.cpp" -o "$OBJ_DIR/$name.o"
done

echo "Compiling orderBook.cpp (including orderHeap.tpp)..."
xcrun clang++ "${CXXFLAGS[@]}" -c "$PROJECT_DIR/src/orderBook.cpp" -o "$OBJ_DIR/orderBook.o"

echo "Linking benchmark..."
xcrun clang++ \
  "$OBJ_DIR/benchmark.o" "$OBJ_DIR/trace.o" "$OBJ_DIR/orderBook.o" \
  -L "$OPENSSL_PREFIX/lib" -lcrypto -o "$BIN"

echo "Generating dSYM..."
xcrun dsymutil "$BIN"

echo "Verifying source-line debug information..."
if ! xcrun dwarfdump --debug-line "$BIN.dSYM" | awk '/Line table prologue/ { found = 1 } END { exit !found }'; then
  echo "Error: dSYM has no usable DWARF line table." >&2
  exit 1
fi

printf '\nBuild successful.\n\nIn Instruments, select this executable:\n%s\n\nMatching symbols:\n%s.dSYM\n\n' "$BIN" "$BIN"
printf 'Keep your existing launch arguments and record a new run.\n'
