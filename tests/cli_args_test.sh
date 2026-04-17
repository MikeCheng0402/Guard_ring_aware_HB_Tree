#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

bin="$tmpdir/placement_test"

g++ -I ./inc -std=c++17 -O2 -Wall \
  main.cpp src/*.cpp -o "$bin" -lreadline -lcurses

output_file="$tmpdir/test_out.txt"

"$bin" ./Spice_file/test.sp "$output_file" >"$tmpdir/stdout.txt" 2>"$tmpdir/stderr.txt"

if [[ ! -f "$output_file" ]]; then
  echo "expected output file to be created for two-argument mode" >&2
  cat "$tmpdir/stdout.txt" >&2 || true
  cat "$tmpdir/stderr.txt" >&2 || true
  exit 1
fi

if [[ ! -s "$output_file" ]]; then
  echo "expected output file to be non-empty" >&2
  exit 1
fi
