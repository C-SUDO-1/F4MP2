# Slice 3I — Minimal Thunk Smoke Test

Purpose:
- keep the 15-byte live patch and realized trampoline
- strip the manual detour thunk down to marker counters plus immediate trampoline call
- avoid callback args, bridge dispatch, player-state normalization, and file tracing on the hot path

What changed:
- `phs_create_player_controls_manual_detour_thunk(...)` now only:
  - increments `detour_entry_count`
  - increments `detour_bridge_forward_count` if a trampoline exists
  - immediately calls the realized trampoline
- removed hot-path calls to `phs_note_manual_detour_entry(...)` and `append_detour_entry_trace(...)` from this thunk

Expected readout:
- launch succeeds + counters rise => raw patch/thunk/trampoline mechanics work; richer logic was the crash source
- crash persists + counters remain zero => fault is still in ABI/thunk/trampoline mechanics before safe bookkeeping is observed
