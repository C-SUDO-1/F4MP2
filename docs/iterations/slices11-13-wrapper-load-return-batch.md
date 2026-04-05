# Slices 11-13 - Wrapper Load Return Truth / Post-Load Isolation

## Slice 11 - wrapper/load return truth
- Adds `f4mp_wrapper_trace.txt` with explicit export-stage markers for `F4SEPlugin_Query` and `F4SEPlugin_Load`.
- Records internal query/load results, prearm state, probe-only state, and the wrapper's intended return path.

## Slice 12 - fatal-after-success isolation
- If internal load succeeds and install state is probe-only active, `F4SEPlugin_Load` now short-circuits and returns true before optional post-load work.
- Override with `F4MP_FORCE_POSTLOAD_CONTINUE=1` to continue the old post-load path for comparison.

## Slice 13 - query/load symmetry trace
- Query now writes wrapper stages too, so prearm and load behavior can be compared in one file.
- Adds explicit markers for manual trace writes, prearm completion, and C++ exception catch points.

## Expected next evidence
- `f4mp_load_trace.txt` should still show `load_ok=1`.
- `f4mp_wrapper_trace.txt` should show whether the export reaches `load_probe_only_short_circuit` and/or `load_return_true`.
- If F4SE still reports fatal disable after `load_probe_only_short_circuit`, the blocker is likely at or beyond the export boundary rather than in optional post-load work.
