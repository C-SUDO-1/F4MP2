# Slice 3E — ABI-Compatible Detour Thunk

This slice adds a target-specific detour thunk for the manual `CreatePlayerControlHandlers` path.

## What changed
- add `phs_create_player_controls_manual_detour_thunk(...)`
- store realized trampoline address in `player_hook_shim`
- route patch realization to the new thunk for `CreatePlayerControlHandlers`
- surface the thunk as the planned detour entry in install/runtime traces

## Intent
Convert the previous `incompatible_detour_entry` failure into a patchable detour destination for the manual controls target.

## Important limit
This thunk is a narrow x64 forwarding seam for the current manual target, not a generalized hook ABI layer.
