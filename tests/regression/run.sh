#!/usr/bin/env bash
#
# Regression test for bufrtotac: decodes the sample BUFR files in examples/
# and compares the TAC output against golden files stored in golden/.
#
# Usage:
#   tests/regression/run.sh            # check current output against golden files
#   tests/regression/run.sh --record   # (re)generate golden files from current output
#
# Environment overrides:
#   BUFR2TAC_TEST_BIN     path to the bufrtotac binary to test
#                         (default: build/src/apps/bufrtotac or build0/src/apps/bufrtotac)
#   BUFR2TAC_TEST_TABLES  path to the bufr tables directory (default: share/)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
EXAMPLES_DIR="$REPO_ROOT/examples"
GOLDEN_DIR="$SCRIPT_DIR/golden"
TABLES_DIR="${BUFR2TAC_TEST_TABLES:-$REPO_ROOT/share}"
# bufrtotac requires the tables dir argument to end with '/'
[[ "$TABLES_DIR" == */ ]] || TABLES_DIR="$TABLES_DIR/"

MODE="check"
if [ "${1:-}" = "--record" ]; then
    MODE="record"
elif [ $# -gt 0 ]; then
    echo "usage: $0 [--record]" >&2
    exit 2
fi

BUFRTOTAC="${BUFR2TAC_TEST_BIN:-}"
if [ -z "$BUFRTOTAC" ]; then
    for candidate in "$REPO_ROOT/build/src/apps/bufrtotac" "$REPO_ROOT/build0/src/apps/bufrtotac"; do
        if [ -x "$candidate" ]; then
            BUFRTOTAC="$candidate"
            break
        fi
    done
fi

if [ -z "$BUFRTOTAC" ] || [ ! -x "$BUFRTOTAC" ]; then
    echo "error: bufrtotac binary not found. Build the project first (see CLAUDE.md)," >&2
    echo "       or point BUFR2TAC_TEST_BIN at an existing binary." >&2
    exit 2
fi

# Sample files known to decode to TAC (sn.0000.bin is a NOAA archive for bufrnoaa, not bufrtotac)
FILES=(
    "20141018211119_ISIN03_EGRR_182100.bufr"
    "20150705121512_ISCD01_LIIB_050000.bufr"
    "20160402121749_IUSH01_DRRN_021100.bufr"
)

mkdir -p "$GOLDEN_DIR"

tmp_out="$(mktemp)"
tmp_diff="$(mktemp)"
trap 'rm -f "$tmp_out" "$tmp_diff"' EXIT

status=0
for name in "${FILES[@]}"; do
    input="$EXAMPLES_DIR/$name"
    golden="$GOLDEN_DIR/$name.tac"

    if [ ! -f "$input" ]; then
        echo "FAIL $name: input file not found at $input"
        status=1
        continue
    fi

    "$BUFRTOTAC" -i "$input" -t "$TABLES_DIR" > "$tmp_out"

    if [ "$MODE" = "record" ]; then
        cp "$tmp_out" "$golden"
        echo "recorded $name -> ${golden#"$REPO_ROOT"/}"
        continue
    fi

    if [ ! -f "$golden" ]; then
        echo "FAIL $name: no golden file yet, run '$0 --record' first"
        status=1
        continue
    fi

    if diff -u "$golden" "$tmp_out" > "$tmp_diff" 2>&1; then
        echo "PASS $name"
    else
        echo "FAIL $name: output differs from golden file"
        cat "$tmp_diff"
        status=1
    fi
done

exit $status
