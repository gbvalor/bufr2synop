# Regression tests

Runs `bufrtotac` against sample BUFR files and diffs the output against golden files, to
catch unintended behavior changes in `libbufrdeco`/`libbufr2tac` when refactoring. Runs
automatically in CI (`.github/workflows/ci.yml`) against both build systems; run it
manually too after local changes:

```
tests/regression/run.sh            # check
tests/regression/run.sh --record   # (re)generate golden files after a reviewed, intentional change
```

By default it looks for `bufrtotac` in `build/src/apps/` or `build0/src/apps/`, and uses
`share/` for the tables. Override with `BUFR2TAC_TEST_BIN` / `BUFR2TAC_TEST_TABLES` if
testing a different build tree.

## Layout

- `cases/<name>.case` — one test per file. Each defines:
  - `INPUT` — a BUFR filename, looked up first in `inputs/`, then in `examples/`.
  - `ARGS` — extra `bufrtotac` flags beyond `-i`/`-t` (may be empty). **Quote it** if it has
    more than one word, e.g. `ARGS="-n -3"` — otherwise the shell parses `ARGS=-n -3` as
    "run the command `-3` with `ARGS=-n` set", not as an assignment.
  - `GOLDEN` — filename of the expected stdout, under `golden/`.
  - `GOLDEN_ERR` — optional filename of the expected stderr, under `golden/`. Set this for a
    case where the interesting output is on stderr (e.g. `-D 1`/`-D 2` debug logging, or a
    strict-mode `-p 0` case that produces no TAC at all). stdout and stderr are captured and
    compared separately, never merged — a merged `2>&1` capture's byte order between the two
    streams depends on libc buffering, which differs between Linux and macOS, and CI runs both.
- `inputs/` — BUFR files that exist only for testing. Reuse a file already in `examples/`
  instead of duplicating it here when one already exercises what you need — `examples/` is
  also what the README's usage walkthrough and `make install` ship.
- `golden/` — recorded expected output, one file per case.

## Adding a case

1. Get the BUFR file into `inputs/` (or point `INPUT` at an existing one in `examples/`).
2. Add `cases/<name>.case` with `INPUT`, `ARGS`, `GOLDEN`.
3. `tests/regression/run.sh --record` to generate the golden file.
4. **Read the generated golden output and confirm it's actually correct** — `--record`
   only captures what the current code produces, not what's meteorologically right. This
   is the step that turns a regression check into a real correctness check; skipping it
   just freezes whatever the code already does, bugs included.
5. `git add` the new BUFR/case/golden files (not done automatically — see repo convention
   of not staging changes without being asked).
