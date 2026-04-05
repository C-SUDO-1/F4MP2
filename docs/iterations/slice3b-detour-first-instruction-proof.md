# Slice 3B - Detour First-Instruction Proof

Purpose:
- separate "candidate selected and armed" from "detour entrypoint actually executed"
- capture a first-instruction-style detour entry seam before snapshot conversion and callback normalization

What changed:
- added `phs_note_manual_detour_entry(const char* site_label)` to `player_hook_shim`
- added `PlayerHookShimStats` and surfaced shim-side detour counters in live-candidate and wiring traces
- added a dedicated `f4mp_detour_entry_trace.txt`
- recorded thread id and return address metadata when detour/shim entry helpers are invoked

Interpretation for the next run:
- if `detour_entry_count == 0` and `detour_bridge_forward_count == 0`, the manual candidate is still only selected/armed on paper
- if `detour_entry_count > 0` but `detour_bridge_forward_count == 0`, the raw detour entry seam is hit but it never advances into the shim bridge
- if `detour_bridge_forward_count > 0` while real callback counters remain zero, the bridge is entered but the real callback path is still broken downstream
