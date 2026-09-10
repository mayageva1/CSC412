#!/usr/bin/env bash

set -euo pipefail

bash Scripts/build.sh

if [[ $# -ne 2 ]]; then
  echo "Usage: bash Scripts/script05v1.sh <data_folder> <output_folder>" >&2
  exit 1
fi
DATA="$1"
OUT="$2"

if [[ ! -d "$DATA" ]]; then
  echo "error: data folder not found: $DATA" >&2
  exit 1
fi


PROG=""
if   [[ -x ./prog05v3 ]]; then PROG="./prog05v3"
elif [[ -x ./prog05v2 ]]; then PROG="./prog05v2"
else
  echo "error: need prog05v3 or prog05v2 in project root" >&2
  exit 1
fi

mkdir -p "$OUT"
rm -f "$OUT"/*.txt 2>/dev/null || true

#  collect .task files 
shopt -s nullglob
TASKS=( "$DATA"/*.task )

if (( ${#TASKS[@]} == 0 )); then
  echo "No .task files found in $DATA"
  exit 0
fi

declare -a PIDS=()

for TASK in "${TASKS[@]}"; do
  # Strip Windows CRs on read
  MODE="$(sed -n '1p' "$TASK" | tr -d '\r')"
  MAPF="$(sed -n '2p' "$TASK" | tr -d '\r')"
  COUNT_LINE="$(sed -n '3p' "$TASK" | tr -d '\r')"

  # Ignore END files (those are v2+)
  if [[ "$MODE" == "END" || "$MAPF" == "END" || "$COUNT_LINE" == "END" ]]; then
    echo "Ignoring END task in v1: $TASK"
    continue
  fi

  # Basic validation
  if [[ -z "$MODE" || -z "$MAPF" || -z "$COUNT_LINE" ]]; then
    echo "skip $TASK (missing header lines)"
    continue
  fi
  if [[ "$MODE" != "TRACE" && "$MODE" != "NO_TRACE" ]]; then
    echo "skip $TASK (line 1 must be TRACE or NO_TRACE; got '$MODE')"
    continue
  fi
  if ! [[ "$COUNT_LINE" =~ ^[0-9]+$ ]]; then
    echo "skip $TASK (COUNT not an integer: '$COUNT_LINE')"
    continue
  fi

  COUNT="$COUNT_LINE"

  # Build ARGS for program
  ARGS=()
  [[ "$MODE" == "TRACE" ]] && ARGS+=( "-t" )
  ARGS+=( "$MAPF" )

  # Read exactly COUNT lines of "r c"
  starts=()
  i=0
  # Read from line 4 to end, but we stop after COUNT valid lines
  # Use 'tail -n +4' to start from line 4; strip CR just in case
  while IFS= read -r line; do
    line="${line%$'\r'}"
    [[ -z "$line" ]] && continue

    # Parse two integers safely
    read -r r c <<<"$line" || true
    if [[ "${r:-}" =~ ^-?[0-9]+$ && "${c:-}" =~ ^-?[0-9]+$ ]]; then
      starts+=( "$r" "$c" )
      (( i++ ))
      (( i >= COUNT )) && break
    else
      echo "skip $TASK (bad start line: '$line')"
      # Force mismatch to error out below
      i=$((COUNT - 1))
      break
    fi
  done < <(tail -n +4 "$TASK")

  if (( i != COUNT )); then
    echo "skip malformed: $TASK "
    continue
  fi

  ARGS+=( "${starts[@]}" )
  ARGS+=( "$OUT" )
  "$PROG" "${ARGS[@]}" &
  PIDS+=( "$!" )
done

# Wait for children (only if any ran)
if (( ${#PIDS[@]} > 0 )); then
  for pid in "${PIDS[@]}"; do
    wait "$pid"
  done
fi

