# Slice 3A — Real Manual Entry Bridge

## Purpose
Move from:
- manual candidate selected and armed
- synthetic validation path working
- no proof that the live engine ever enters the armed manual site

to:
- the manual shim boundary routes into the explicit real callback path
- real entry leaves dedicated traces even before higher-level validation succeeds
- the next run can distinguish `installed but dead` from `installed and entered`

## Scope
- Bridge `player_hook_shim` into `clf4_player_hook_callback_real_emit(...)`.
- Keep the existing remote-forward path intact.
- Add explicit shim-side counters and a dedicated `f4mp_real_entry_bridge_trace.txt`.
- Preserve `f4mp_real_entry.txt` as the first-hit sentinel.

## Exit criteria
- If the armed manual site is never entered, then:
  - `f4mp_real_entry.txt` stays absent or unchanged
  - `f4mp_real_entry_bridge_trace.txt` stays absent or unchanged
  - `real_forward/real_accept/real_reject` remain zero
- If the armed manual site is entered, then:
  - `f4mp_real_entry.txt` records hits
  - `f4mp_real_entry_bridge_trace.txt` records `real_ingest`
  - `clf4_player_hook_callback_real_emit(...)` is attempted from the shim boundary
  - downstream real counters can rise naturally
