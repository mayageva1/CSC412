#!/usr/bin/env bash

set -euo pipefail

declare -a ENTRIES=(
  "Programs/Version1/prog05v1.cpp:prog05v1"
  "Programs/Version2/prog05v2.cpp:prog05v2"
  "Programs/Version3/prog05v3.cpp:prog05v3"
  "Programs/Version4/prog05v4.cpp:prog05v4"
)

for entry in "${ENTRIES[@]}"; do
  SRC="${entry%%:*}"
  OUT="${entry##*:}"
  if [[ -f "$SRC" ]]; then
    g++ -std=gnu++20 -O2 -Wall -Wextra -o "$OUT" "$SRC"
  fi
done

