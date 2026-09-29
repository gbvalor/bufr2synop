#!/usr/bin/env bash
#
# Regression test for bufrtotac. Each file in cases/*.case describes one test:
# which sample BUFR file to decode, which extra bufrtotac options to use, and
# which golden file in golden/ holds the expected output.
#
# Usage:
#   tests/regression/run.sh            # check current output against golden files
#   tests/regression/run.sh --record   # (re)generate golden files from current output
#
# Environment overrides:
#   BUFR2TAC_TEST_BIN     path to the bufrtotac binary to test
#                         (default: build/src/apps/bufrtotac or build0/src/apps/bufrtotac)
#   BUFR2TAC_TEST_TABLES  path to the bufr tables directory (default: share/)
#
# Adding a case: drop the source BUFR in inputs/ (test-only fixtures) or reuse
# one already in examples/, then add cases/<name>.case defining INPUT, ARGS
# (extra bufrtotac flags beyond -i/-t, may be empty; QUOTE it if it has more
# than one word, e.g. ARGS="-n -3", otherwise the shell parses it as
# "run the command -3 with ARGS=-n set"), GOLDEN (filename under golden/ for
# stdout) and optionally GOLDEN_ERR (filename under golden/ for stderr, e.g.
# for a strict-mode case that produces no TAC and only logs to stderr).
# stdout and stderr are captured and compared separately, never merged: a
# `2>&1` capture's byte order between the two streams depends on libc
# buffering, which differs between Linux and macOS, and this suite runs CI on
# both.
# Run with --record and review the golden output by hand before trusting it
# — recording only captures "what the code does now", not
# "what is correct".

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
CASES_DIR="$SCRIPT_DIR/cases"
INPUTS_DIR="$SCRIPT_DIR/inputs"
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

mkdir -p "$GOLDEN_DIR"

shopt -s nullglob
case_files=("$CASES_DIR"/*.case)
shopt -u nullglob

if [ ${#case_files[@]} -eq 0 ]; then
    echo "error: no test cases found in $CASES_DIR" >&2
    exit 2
fi

tmp_out="$(mktemp)"
tmp_err="$(mktemp)"
tmp_diff="$(mktemp)"
trap 'rm -f "$tmp_out" "$tmp_err" "$tmp_diff"' EXIT

# Compares $2 (actual) against $1 (golden), labeling failures with $3/$4
# (stream name / case name). Prints a diff and returns 1 on mismatch.
check_stream() {
    local golden_path="$1" actual_path="$2" label="$3" name="$4"
    if [ ! -f "$golden_path" ]; then
        echo "FAIL $name ($label): no golden file yet, run '$0 --record' first"
        return 1
    fi
    if diff -u "$golden_path" "$actual_path" > "$tmp_diff" 2>&1; then
        return 0
    fi
    echo "FAIL $name ($label): output differs from golden file"
    cat "$tmp_diff"
    return 1
}

status=0
for case_file in "${case_files[@]}"; do
    case_name="$(basename "$case_file" .case)"

    # Reset per-case variables so a case that forgets to set one doesn't
    # silently inherit it from the previous case in the loop.
    INPUT=""
    ARGS=""
    GOLDEN=""
    GOLDEN_ERR=""
    # shellcheck disable=SC1090
    source "$case_file"

    if [ -z "$INPUT" ] || [ -z "$GOLDEN" ]; then
        echo "FAIL $case_name: case file must set INPUT and GOLDEN"
        status=1
        continue
    fi

    input_path=""
    for dir in "$INPUTS_DIR" "$EXAMPLES_DIR"; do
        if [ -f "$dir/$INPUT" ]; then
            input_path="$dir/$INPUT"
            break
        fi
    done

    if [ -z "$input_path" ]; then
        echo "FAIL $case_name: input '$INPUT' not found in $INPUTS_DIR or $EXAMPLES_DIR"
        status=1
        continue
    fi

    golden="$GOLDEN_DIR/$GOLDEN"

    # Run with cwd = repo root and pass a repo-relative -i path: bufrtotac echoes the -i
    # argument verbatim into e.g. JSON output ("bufrfile"), so an absolute path here would
    # bake this machine's checkout location into the golden file and break on any other one.
    rel_input="${input_path#"$REPO_ROOT"/}"

    # Intentionally unquoted: ARGS is a space-separated list of extra flags
    # (e.g. "-p 0"), and this avoids bash-3.2's "unbound variable" trap on
    # empty arrays under `set -u` (macOS ships bash 3.2 as /bin/bash).
    ( cd "$REPO_ROOT" && "$BUFRTOTAC" -i "$rel_input" -t "$TABLES_DIR" $ARGS ) > "$tmp_out" 2> "$tmp_err"

    if [ "$MODE" = "record" ]; then
        cp "$tmp_out" "$golden"
        echo "recorded $case_name (stdout) -> ${golden#"$REPO_ROOT"/}"
        if [ -n "$GOLDEN_ERR" ]; then
            golden_err="$GOLDEN_DIR/$GOLDEN_ERR"
            cp "$tmp_err" "$golden_err"
            echo "recorded $case_name (stderr) -> ${golden_err#"$REPO_ROOT"/}"
        fi
        continue
    fi

    case_status=0
    check_stream "$golden" "$tmp_out" "stdout" "$case_name" || case_status=1
    if [ -n "$GOLDEN_ERR" ]; then
        check_stream "$GOLDEN_DIR/$GOLDEN_ERR" "$tmp_err" "stderr" "$case_name" || case_status=1
    fi

    if [ "$case_status" -eq 0 ]; then
        echo "PASS $case_name"
    else
        status=1
    fi
done

exit $status
