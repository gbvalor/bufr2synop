# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this project is

`bufr2synop` converts meteorological reports from **BUFR** (binary) format into the legacy
**TAC** (Traditional Alphanumeric Code) formats: SYNOP (FM 12), SHIP (FM 13), SYNOP MOBIL (FM 14),
BUOY (FM 18), TEMP/TEMP SHIP/TEMP MOBIL (FM 35/36/38), CLIMAT (FM 71). The conversion is not
guaranteed to match the original alphanumeric report byte-for-byte — some TAC code table values
are ambiguous and depend on regional/national conventions that cannot be inferred from BUFR alone.

Pure C (C99), GPLv2+. No test suite, no CI currently exists in the repository.

## Build

Two parallel build systems are maintained (both must keep working — CI/packaging depend on both):

**Autotools** (first checkout only, requires autotools installed):
```
make -f Makefile.cvs      # generates configure, run once after clone
mkdir build0 && cd build0
../configure
make
```

**CMake**:
```
mkdir build && cd build
cmake ..
make
```

There is no `make check` / test target and no lint target configured. Warnings are treated
seriously: both build systems compile with `-Wall -Wextra -W` (see `configure.ac` and
`CMakeLists.txt`), so new code should be warning-clean under both GCC and Clang.

## Regression testing

`tests/regression/run.sh` decodes the sample BUFR files in `examples/` with `bufrtotac` and diffs
the TAC output against golden files in `tests/regression/golden/`. Run it after building, before
and after changes to `libbufrdeco`/`libbufr2tac`:

```
tests/regression/run.sh            # check
tests/regression/run.sh --record   # re-record golden files after an intentional output change
```

Runs automatically in CI (`.github/workflows/ci.yml`) against both build systems; run it manually
too after local changes. See `tests/regression/README.md`.

Version number is set in exactly two places that must stay in sync: `configure.ac` (`AC_INIT`)
and `CMakeLists.txt` (`project(... VERSION ...)`).

## Architecture

The pipeline is a strict two-stage decode:

```
BUFR file --> libbufrdeco (generic BUFR decode) --> libbufr2tac (BUFR-semantics-aware TAC encode) --> TAC text
```

- **`src/bufrdeco/`** — `libbufrdeco`. A general-purpose BUFR decoder with no knowledge of
  meteorological report semantics. Entry points: `bufrdeco_init()`, `bufrdeco_read_bufr()` (reads
  sec0–sec4, parses/expands the descriptor tree, decodes data into
  `struct bufrdeco.seq`), `bufrdeco_close()`. The central struct is `struct bufrdeco` (in
  `bufrdeco.h`), which holds the parsed sections (`sec0`..`sec4`), the expanded descriptor tree,
  the decoded subset sequence data, and the tables in use. It supports two optional performance
  features (see README): an in-memory cache of BUFR tables (`bufr_tables_cache`, useful when
  processing many files whose master table version differs) and a bit-offset index file
  (`.offs` sidecar, `-R`/`-W` in `bufrtotac`) to jump directly to a given subset of a
  non-compressed BUFR message instead of re-parsing sec4 from the start.
- **`src/libraries/`** — `libbufr2tac` (named `libbufr2synop` before version 0.7). Consumes the
  decoded `struct bufrdeco_subset_sequence_data` and turns it into TAC. Report-type dispatch
  happens in `bufr2tac_synop.c` / `_buoy.c` / `_temp.c` / `_climat.c`, called through
  `parse_subset_sequence()`. Individual BUFR descriptor classes (0-01, 0-02, 0-04, ... 0-33) each
  have their own handler file `bufr2tac_x01.c`, `bufr2tac_x02.c`, etc. — when adding support for a
  new BUFR descriptor, this is where it's decoded into the relevant `*_chunks` struct field.
  Output formatting per report type lives in `bufr2tac_print_synop.c` / `_buoy.c` / `_temp.c` /
  `_climat.c`. The result of a full parse is accumulated into `struct metreport` (`bufr2tac.h`),
  which can hold up to 4 report parts (`alphanum`..`alphanum4`) since some BUFR messages
  decode into multi-part TAC reports (e.g. TEMP parts A/B/C/D).
- **`src/apps/`** — CLI binaries, thin wrappers around the two libraries:
  - `bufrtotac.c`/`bufrtotac_io.c` — main binary; drives `bufrdeco_read_bufr()` then
    `bufrtotac_parse_subset_sequence()` per subset, with output in TAC/JSON/XML/CSV/HTML.
  - `bufrdeco_json.c` — inspects a BUFR file via `libbufrdeco` only (sections, expanded tree,
    data) without any TAC conversion.
  - `bufrnoaa.c`/`bufrnoaa_io.c`/`bufrnoaa_utils.c` — extracts/filters individual BUFR messages
    out of NOAA GTS gateway `.bin` archives (unrelated to the decode pipeline itself).
  - `build_bufrdeco_tables.c` — converts WMO/ECMWF master BUFR tables into the CSV format
    consumed by `libbufrdeco` (output lives in `share/`); internal tooling, not for end users.
  - `eccodes_local_to_bufrdeco.c` — converts ecCodes-format local tables
    (`element.table`, `sequence.def`, `codetables/*.table`) into bufrdeco's
    `tableB/C/D_LOCAL_<local>_<centre>_<subcentre>.csv` format.
- **`share/`** — BUFR master tables (B/C/D) as CSV, one directory per master table version
  (`BUFR_<version>_<local>_<...>`), plus `share/common/` and `share/local/` for shared/local
  tables. These are generated data files, not hand-edited; regenerate via
  `build_bufrdeco_tables`/`eccodes_local_to_bufrdeco` rather than editing CSVs directly.
- **`src/scripts/`** — shell helper scripts (`prepare_TableA/B/C/D`, `bufrdeco_check`,
  `fix_double_quotes`) used when preparing/checking table data, not part of the runtime pipeline.
- **`examples/`** — sample BUFR files referenced by the README's usage walkthrough; useful as
  manual regression inputs when there's no automated test suite (diff `bufrtotac -i <file>`
  output before/after a change).

## Conventions specific to this codebase

- Global mutable state in `bufrtotac.c` (e.g. `struct bufrdeco BUFR`, `struct metreport REPORT`)
  is intentional for this single-threaded CLI tool — don't refactor it into thread-safe/reentrant
  form without a stated reason.
- Error/warning reporting in `libbufr2tac` uses an explicit stack (`struct bufr2tac_error_stack`,
  `bufr2tac_push_error()`/`bufr2tac_set_error()`), not `errno`/exceptions — follow this pattern
  when adding new decode paths. `libbufrdeco` reports errors via the `char error[1024]` field on
  `struct bufrdeco`.
- `-p` permissive/strict mode (`bufr2tac_set_strict_mode()`) controls whether malformed/ambiguous
  values raise a hard error or a best-effort fallback — keep new validation logic consistent with
  whichever mode is active rather than always failing hard.
