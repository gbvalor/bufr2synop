# Regression tests

Compares `bufrtotac` TAC output for the sample BUFR files in `examples/` against
golden files stored in `golden/`. This catches unintended behavior changes in
`libbufrdeco`/`libbufr2tac` when refactoring.

Not wired into a build target yet (no `make check`/CI); run manually after building:

```
tests/regression/run.sh
```

If the output has legitimately changed (new feature, intentional fix), review the
diff and re-record:

```
tests/regression/run.sh --record
```

By default it looks for `bufrtotac` in `build/src/apps/` or `build0/src/apps/`, and
uses `share/` for the tables. Override with `BUFR2TAC_TEST_BIN` / `BUFR2TAC_TEST_TABLES`
if testing a different build tree.
