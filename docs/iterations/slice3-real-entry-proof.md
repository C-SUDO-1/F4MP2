# Slice 3 — Real Entry Proof Plumbing

## Purpose
Move from:
- candidate selected and armed
- synthetic validation sample accepted
- no proof of real engine execution

to:
- synthetic and real callback paths are distinct in code and traces
- real entry evidence is explicit, not inferred from callback counts
- next run can distinguish `installed but dead` from `installed and executing`

## Scope
- Add an explicit player-hook dispatch context (`synthetic_validation` vs `real_hook`).
- Add a real callback dispatch API: `clf4_player_hook_callback_real_emit(...)`.
- Track real callback counters separately from total counters.
- Update install/live/wiring traces to consume explicit real counters.
- Stop writing synthetic normalization events into `f4mp_real_hit.txt`.

## Exit criteria
- A normal validation run still shows:
  - selected manual candidate
  - synthetic validation success
  - `real_callback_evidence=0`
  - `real_forward/real_accept/real_reject = 0`
- A future real detour/hook call routed through `clf4_player_hook_callback_real_emit(...)` produces:
  - `f4mp_real_entry_trace.txt`
  - `f4mp_real_hit.txt` entries with `dispatch=real_hook`
  - non-zero `real_forward`/`real_accept` or `real_reject`
  - `real_callback_evidence=1`

## Files changed
- `plugin/include/commonlibf4_player_hook_dispatch_context.h`
- `plugin/src/commonlibf4_player_hook_dispatch_context.c`
- `plugin/include/commonlibf4_player_hook_callback_stub.h`
- `plugin/src/commonlibf4_player_hook_callback_stub.c`
- `plugin/include/commonlibf4_player_hook_source_candidate_adapter.h`
- `plugin/src/commonlibf4_player_hook_source_candidate_adapter.c`
- `plugin/include/commonlibf4_player_hook_install_candidate.h`
- `plugin/src/commonlibf4_player_hook_install_candidate.c`
- `plugin/include/commonlibf4_player_hook_live_callback_candidate.h`
- `plugin/src/commonlibf4_player_hook_live_callback_candidate.c`
- `plugin/src/commonlibf4_player_live_adapter.c`
- `plugin/src/phase1r_real_local_hook_wiring.c`
- `plugin/src/f4se_plugin_exports.cpp`
- `CMakeLists.txt`
