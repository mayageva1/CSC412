in_dir="$1"
scrap_dir="$2"
out_dir="$3"

# track if we created scrap_dir so we know whether to remove it later
created_scrap=0
if [ ! -d "$scrap_dir" ]; then
  mkdir -p "$scrap_dir"
  created_scrap=1
fi

# ensure output dir
mkdir -p "$out_dir"


shopt -s nullglob
txt_files=("$in_dir"/*.txt)


# ---- convert each file line-by-line ----
for src in "${txt_files[@]}"; do
  base="$(basename "$src")"
  tmp="$scrap_dir/$base.tmp"
  dst="$out_dir/$base"

  : > "$tmp"
  # Preserve spaces/backslashes; handle last line without trailing newline
  while IFS= read -r line || [ -n "$line" ]; do
    line="${line%$'\r'}"         # strip trailing CR if present
    printf '%s\n' "$line" >> "$tmp"
  done < "$src"

  mv -f "$tmp" "$dst"
  echo "Converted: $src -> $dst"
done

# ---- cleanup scrap only if we created it ----
if [ "$created_scrap" -eq 1 ]; then
  rm -rf "$scrap_dir"
fi

echo "Done."