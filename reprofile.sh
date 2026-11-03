#!/usr/bin/env bash
set -euo pipefail

# Build a fresh Instruments profiling binary from the current Polymarkets sources.
# Install this script as polymarket/reprofile.sh and run ./reprofile.sh from anywhere.
#
# The executable selected in Instruments always remains at:
#     polymarket/profiles/current/custom-benchmark
# Every build also gets an immutable copy with its matching dSYM under profiles/builds/.

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(cd "$PROJECT_DIR/../polymarket-base" && pwd)"
PROFILE_DIR="$PROJECT_DIR/profiles"
NATIVE_DIR="$PROJECT_DIR/benchmark-native-2026-10-19/native"
JSON_INCLUDE="$BASE_DIR/cpp/build/_deps/nlohmann_json-src/include"

if ! command -v brew >/dev/null 2>&1; then
  echo "Error: Homebrew is required to find OpenSSL 3." >&2
  exit 1
fi
OPENSSL_PREFIX="$(brew --prefix openssl@3)" || {
  echo "Error: OpenSSL 3 is missing. Install with: brew install openssl@3" >&2
  exit 1
}

for src in \
  "$NATIVE_DIR/benchmark.cpp" \
  "$NATIVE_DIR/trace.cpp" \
  "$PROJECT_DIR/src/orderBook.cpp" \
  "$PROJECT_DIR/src/hotpath/bookside.cpp" \
  "$PROJECT_DIR/src/hotpath/orderPool.cpp" \
  "$JSON_INCLUDE/nlohmann/json.hpp" \
  "$OPENSSL_PREFIX/include/openssl/evp.h"; do
  if [[ ! -f "$src" ]]; then
    echo "Error: missing $src" >&2
    exit 1
  fi
done

BUILD_ID="$(date +%Y%m%d-%H%M%S)-$$"
BUILD_DIR="$PROFILE_DIR/builds/$BUILD_ID"
OBJ_DIR="$BUILD_DIR/obj"
BUILD_EXE="$BUILD_DIR/custom-benchmark"
CURRENT_DIR="$PROFILE_DIR/current"
CURRENT_EXE="$CURRENT_DIR/custom-benchmark"
CURRENT_DSYM="$CURRENT_EXE.dSYM"
mkdir -p "$OBJ_DIR" "$CURRENT_DIR"

CXXFLAGS=(
  -std=c++20 -O3 -DNDEBUG -g -fno-omit-frame-pointer
  -I "$BASE_DIR/cpp/src"
  -I "$JSON_INCLUDE"
  -I "$PROJECT_DIR/src"
  -I "$OPENSSL_PREFIX/include"
  -I "$(brew --prefix boost)/include"
)

cd "$BASE_DIR"
for name in benchmark trace; do
  echo "Compiling $name.cpp..."
  xcrun clang++ "${CXXFLAGS[@]}" -c "$NATIVE_DIR/$name.cpp" -o "$OBJ_DIR/$name.o"
done

ENGINE_OBJS=()
for src in orderBook hotpath/bookside hotpath/orderPool; do
  echo "Compiling $src.cpp..."
  obj="$OBJ_DIR/$(basename "$src").o"
  xcrun clang++ "${CXXFLAGS[@]}" -c "$PROJECT_DIR/src/$src.cpp" -o "$obj"
  ENGINE_OBJS+=("$obj")
done

echo "Linking..."
xcrun clang++ \
  "$OBJ_DIR/benchmark.o" "$OBJ_DIR/trace.o" "${ENGINE_OBJS[@]}" \
  -L "$OPENSSL_PREFIX/lib" -lcrypto -o "$BUILD_EXE"

echo "Generating debug symbols..."
xcrun dsymutil "$BUILD_EXE"

# An executable with recognizable function names is not enough for source-level
# profiling: ensure the dSYM actually contains DWARF file/line mappings.
if ! xcrun dwarfdump --debug-line "$BUILD_EXE.dSYM" | awk \
  '/Line table prologue/ { found=1 } END { exit !found }'; then
  echo "Error: dSYM is missing source-line tables. Current executable was not replaced." >&2
  exit 1
fi

# The binary and its dSYM must describe exactly the same Mach-O build.
BIN_UUID="$(xcrun dwarfdump --uuid "$BUILD_EXE" | awk '/UUID:/ { print $2; exit }')"
DSYM_UUID="$(xcrun dwarfdump --uuid "$BUILD_EXE.dSYM" | awk '/UUID:/ { print $2; exit }')"
if [[ -z "$BIN_UUID" || "$BIN_UUID" != "$DSYM_UUID" ]]; then
  echo "Error: executable/dSYM UUID mismatch. Current executable was not replaced." >&2
  exit 1
fi

# Install to one stable target path. The complete versioned build above remains
# untouched, including matching symbols and persistent object files.
cp -p "$BUILD_EXE" "$CURRENT_EXE.new"
rm -rf "$CURRENT_DSYM.new"
cp -R "$BUILD_EXE.dSYM" "$CURRENT_DSYM.new"
mv -f "$CURRENT_EXE.new" "$CURRENT_EXE"
rm -rf "$CURRENT_DSYM"
mv "$CURRENT_DSYM.new" "$CURRENT_DSYM"

printf '\nBuild successful.\n'
printf 'In Instruments, always launch this same executable:\n%s\n' "$CURRENT_EXE"
printf '\nMatching dSYM:\n%s\n' "$CURRENT_DSYM"
printf '\nPreserved build and symbols:\n%s\n' "$BUILD_DIR"
printf '\nBuild UUID: %s\n' "$BIN_UUID"
printf 'Keep the same benchmark arguments when recording before/after runs.\n'
printf 'This script builds only; it does not run correctness tests or start recording.\n'
