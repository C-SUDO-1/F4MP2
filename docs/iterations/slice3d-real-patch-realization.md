# Slice 3D — Real Patch Realization

Purpose:
- stop reporting the manual controls candidate as installed unless a real patch write and trampoline exist.
- surface concrete patch-realization truth in install, live-candidate, and wiring traces.

What changed:
- added `commonlibf4_player_hook_patch_realization.{h,c}`.
- install attempts now call a real patch-realization module after discovery/runtime gates pass.
- install only returns `HOOK_INSTALL_INSTALLED` when patch realization succeeds.
- current planned detour destination (`phs_note_manual_detour_entry`) is treated as incompatible for direct detour writing, so the patch module fails explicitly instead of pretending install succeeded.
- trace state now includes:
  - detour destination label/address
  - patch realization error code/label
  - platform error code
  - realized patch/trampoline fields

Expected next-run outcome on the current tree:
- selected manual candidate remains visible.
- `patch_step_attempted=1`.
- `install_state` no longer claims installed when no realized detour write exists.
- patch traces should show `patch_realization_error_label=incompatible_detour_entry` until a dedicated ABI-compatible detour thunk is introduced.
