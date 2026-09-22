#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build"
CXX_BIN="${CXX:-clang++}"
ENDSTONE_SOURCE_DIR="${1:-}"

command -v "$CXX_BIN" >/dev/null 2>&1 || {
  echo "ERROR: clang++ not found."
  exit 1
}

ARGS=(
  -S "$ROOT"
  -B "$BUILD"
  -G Ninja
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_CXX_COMPILER="$CXX_BIN"
)

if [[ -n "$ENDSTONE_SOURCE_DIR" ]]; then
  ARGS+=("-DENDSTONE_SOURCE_DIR=$ENDSTONE_SOURCE_DIR")
fi

cmake "${ARGS[@]}"
cmake --build "$BUILD" --target cpsguard -j"$(nproc)"

SO="$(find "$BUILD" -maxdepth 3 -type f -name 'endstone_cpsguard.so' -print -quit)"
if [[ -z "$SO" ]]; then
  echo "ERROR: endstone_cpsguard.so was not produced."
  exit 2
fi
cp -f "$SO" "$ROOT/endstone_cpsguard.so"
echo "Built: $ROOT/endstone_cpsguard.so"
