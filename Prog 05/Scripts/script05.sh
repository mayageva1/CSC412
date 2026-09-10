#!/usr/bin/env bash

set -euo pipefail

#  args 
if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Usage: bash Scripts/script05.sh [-t] <watch_folder> <output_folder>" >&2
  exit 1
fi
if [[ "${1:-}" == "-t" ]]; then shift; fi
WATCH="$1"
OUT="$2"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
bash Scripts/build.sh
[[ -x ./prog05v4 ]] || { echo "error: prog05v4 not built"; exit 1; }

# dirs
mkdir -p "$WATCH" "$OUT"
# optional: clear output .txt files
find "$OUT" -mindepth 1 -maxdepth 1 -exec rm -rf {} + 2>/dev/null || true

declare -a KNOWN_MAPS=()   # holds map basenames (without .map)
declare -a SEEN_PATHS=()   # paths we've already handled

known_has() { local n="$1"; local x; for x in "${KNOWN_MAPS[@]:-}"; do [[ "$x" == "$n" ]] && return 0; done; return 1; }
known_add() { KNOWN_MAPS+=( "$1" ); }

seen_has()  { local n="$1"; local x; for x in "${SEEN_PATHS[@]:-}";  do [[ "$x" == "$n" ]] && return 0; done; return 1; }
seen_add()  { SEEN_PATHS+=( "$1" ); }

TMPDIR="$(mktemp -d "${TMPDIR:-/tmp}/gd.XXXXXX")"
FIFO="$TMPDIR/gd.pipe"
mkfifo "$FIFO"
./prog05v4 "$OUT" <"$FIFO" &
GD_PID=$!
exec 9>"$FIFO"   # FD 9 = general dispatcher's stdin

cleanup() {
  exec 9>&- 2>/dev/null || true
  wait "$GD_PID" 2>/dev/null || true
  rm -f "$FIFO" 2>/dev/null || true
  rmdir "$TMPDIR" 2>/dev/null || true
}
trap cleanup EXIT

echo "Watching: $WATCH  (dispatch via prog05v4)"

# enable nullglob once, outside the loop
shopt -s nullglob

while true; do
  # loop directly over the glob; if there are no matches, the loop is skipped
  for F in "$WATCH"/*; do
    [[ -e "$F" ]] || continue
    seen_has "$F" && continue  # already processed this path

    BASE="$(basename "$F")"
    EXT="${BASE##*.}"

    if [[ "$EXT" == "map" ]]; then
      MAPBASE="${BASE%.map}"
      known_has "$MAPBASE" || known_add "$MAPBASE"
      echo "$F" >&9
      echo "sent map:  $F"
      seen_add "$F"

    elif [[ "$EXT" == "task" ]]; then
      TBASE="${BASE%.task}"
      if ! known_has "$TBASE"; then
        echo "unknown map: $TBASE   (drop the corresponding .map file first)"
        # mark seen to avoid repeated warnings until the user re-drops it
        seen_add "$F"
        continue
      fi
      echo "$F" >&9
      echo "sent task: $F"
      seen_add "$F"

    else
      # ignore other file types, but mark as seen to avoid repeated logs
      seen_add "$F"
    fi
  done
  sleep 1
done
