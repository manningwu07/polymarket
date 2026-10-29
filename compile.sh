#!/usr/bin/env bash
set -euo pipefail

# Build Polymarkets' native C++ benchmark in release mode.
# Place this file at polymarket/compile.sh and run it from any directory.
# This does not modify the official benchmark artifacts or profiling builds.
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(cd "$PROJECT_DIR/../polymarket-base" && pwd)"
NATIVE_DIR="$PROJECT_DIR/benchmark-native-2026-10-19/native"
BUILD_DIR="$PROJECT_DIR/build"
BIN="$BUILD_DIR/custom-benchmark"
JSON_INCLUDE="$BASE_DIR/cpp/build/_deps/nlohmann_json-src/include"

if ! command -v brew >/dev/null 2>&1; then
  echo "Error: Homebrew is required to locate OpenSSL 3." >&2
  exit 1
fi
OPENSSL_PREFIX="$(brew --prefix openssl@3)"

for file in \
  "$NATIVE_DIR/benchmark.cpp" \
  "$NATIVE_DIR/trace.cpp" \
  "$PROJECT_DIR/src/orderBook.cpp" \
  "$PROJECT_DIR/src/orderHandle.cpp" \
  "$JSON_INCLUDE/nlohmann/json.hpp" \
  "$OPENSSL_PREFIX/include/openssl/evp.h"; do
  if [[ ! -f "$file" ]]; then
    echo "Error: missing required file: $file" >&2
    exit 1
  fi
done

mkdir -p "$BUILD_DIR"
TMP_DIR="$(mktemp -d "$BUILD_DIR/.compile.XXXXXX")"
trap 'rm -rf "$TMP_DIR"' EXIT

CXXFLAGS=(
  -std=c++20 -O3 -DNDEBUG
  -I "$BASE_DIR/cpp/src"
  -I "$JSON_INCLUDE"
  -I "$PROJECT_DIR/src"
  -I "$OPENSSL_PREFIX/include"
  -I "$(brew --prefix boost)/include"
)

printf 'Compiling native benchmark sources...\n'
for name in benchmark trace; do
  xcrun clang++ "${CXXFLAGS[@]}" -c "$NATIVE_DIR/$name.cpp" -o "$TMP_DIR/$name.o"
done

printf 'Compiling matching engine (including orderHeap.tpp)...\n'
xcrun clang++ "${CXXFLAGS[@]}" -c "$PROJECT_DIR/src/orderBook.cpp" -o "$TMP_DIR/orderBook.o"
xcrun clang++ "${CXXFLAGS[@]}" -c "$PROJECT_DIR/src/orderHandle.cpp" -o "$TMP_DIR/orderHandle.o"

printf 'Linking...\n'
xcrun clang++ \
  "$TMP_DIR/benchmark.o" "$TMP_DIR/trace.o" "$TMP_DIR/orderBook.o" "$TMP_DIR/orderHandle.o" \
  -L "$OPENSSL_PREFIX/lib" -lcrypto \
  -o "$TMP_DIR/custom-benchmark"

# Only replace the previous successful binary after a full successful build.
mv "$TMP_DIR/custom-benchmark" "$BIN"
printf '\nBuild complete: %s\n' "$BIN"
printf 'This is a release build; use ./reprofile.sh for Instruments source-level profiling.\n'
