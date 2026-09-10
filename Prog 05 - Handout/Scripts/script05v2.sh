#!/usr/bin/env bash
# Usage: bash Scripts/script05v2.sh <watch_folder> <output_folder>

set -euo pipefail

# --- args ---
if [[ $# -ne 2 ]]; then
  echo "Usage: bash Scripts/script05v2.sh <watch_folder> <output_folder>" >&2
  exit 1
fi
WATCH="$1"
OUT="$2"

# --- go to project root & build (per instructor: call bash explicitly) ---
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
bash Scripts/build.sh

# --- pick program (v3 preferred, else v2) ---
PROG=""
if   [[ -x ./prog05v3 ]]; then PROG="./prog05v3"
elif [[ -x ./prog05v2 ]]; then PROG="./prog05v2"
else
  echo "error: need prog05v3 or prog05v2 in project root" >&2
  exit 1
fi

# --- prep folders ---
mkdir -p "$WATCH" "$OUT"
find "$OUT" -mindepth 1 -maxdepth 1 -exec rm -rf {} + 2>/dev/null || true

# --- state (bash 3.2 friendly) ---
declare -a SEEN_LIST=()   # files we've already processed
declare -a PIDS=()        # launched process ids

seen_has() {
  # returns 0 if path is already in SEEN_LIST, else 1
  local needle="$1"
  local n=${#SEEN_LIST[@]}   # safe even if empty
  if (( n == 0 )); then
    return 1
  fi
  local x
  for x in "${SEEN_LIST[@]}"; do
    [[ "$x" == "$needle" ]] && return 0
  done
  return 1
}
seen_add() {
  SEEN_LIST+=( "$1" )
}

echo "Watching: $WATCH  (program: $PROG)"
while true; do
  shopt -s nullglob
  FILES=( "$WATCH"/* )

  for F in "${FILES[@]}"; do
    [[ -e "$F" ]] || continue
    if seen_has "$F"; then
      continue
    fi
    seen_add "$F"

    # ---- END file support (single line "END") ----
    if [[ -f "$F" ]]; then
      content="$(tr -d '\r' < "$F")"
      if [[ "$content" == "END" ]]; then
        echo "END received. Waiting for all launched tasks to finish…"
        if (( ${#PIDS[@]} > 0 )); then
          for pid in "${PIDS[@]}"; do
            if kill -0 "$pid" 2>/dev/null; then
              wait "$pid" || true
            fi
          done
        fi
        echo "All tasks finished. Bye."
        exit 0
      fi
    fi

    # ignore non-task files
    [[ "${F##*.}" == "task" ]] || { echo "ignore (not .task): $F"; continue; }

    # ---- parse strict v2 task format ----
    MODE="$(sed -n '1p' "$F" | tr -d '\r')"
    MAPF="$(sed -n '2p' "$F" | tr -d '\r')"
    COUNT_LINE="$(sed -n '3p' "$F" | tr -d '\r')"

    if [[ -z "$MODE" || -z "$MAPF" || -z "$COUNT_LINE" ]]; then
      echo "bad task (missing header lines): $F"
      continue
    fi
    if [[ "$MODE" != "TRACE" && "$MODE" != "NO_TRACE" ]]; then
      echo "bad task (line 1 must be TRACE or NO_TRACE; got '$MODE'): $F"
      continue
    fi
    if ! [[ "$COUNT_LINE" =~ ^[0-9]+$ ]]; then
      echo "bad task (COUNT not an integer: '$COUNT_LINE'): $F"
      continue
    fi
    COUNT="$COUNT_LINE"

    # build args
    ARGS=()
    [[ "$MODE" == "TRACE" ]] && ARGS+=( "-t" )
    ARGS+=( "$MAPF" )

    # read exactly COUNT lines of "r c"
    starts=()
    i=0
    while IFS= read -r line; do
      line="${line%$'\r'}"
      [[ -z "$line" ]] && continue
      r="" c=""
      read -r r c <<<"$line" || true
      if [[ "$r" =~ ^-?[0-9]+$ && "$c" =~ ^-?[0-9]+$ ]]; then
        starts+=( "$r" "$c" )
        (( i++ ))
        (( i >= COUNT )) && break
      else
        echo "bad task (invalid start line: '$line'): $F"
        # force mismatch so we skip this task
        i=$((COUNT - 1))
        break
      fi
    done < <(sed -n '4,$p' "$F")

    if (( i != COUNT )); then
      echo "bad task (expected $COUNT start lines, got $i): $F"
      continue
    fi

    ARGS+=( "${starts[@]}" )
    ARGS+=( "$OUT" )

    echo "Launching: $PROG ${ARGS[*]}  (from $F)"
    "$PROG" "${ARGS[@]}" &
    PIDS+=( "$!" )
  done

  sleep 1
done
