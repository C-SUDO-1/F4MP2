# Slice 3H — 15-byte Boundary-Aligned Trampoline Realizer

Purpose:
- replace the unsafe fixed 12-byte overwrite assumption with the planner-derived whole-instruction overwrite span
- realize the manual `CreatePlayerControlHandlers` detour only when the decoded prologue is supported and relocation-safe

What changed:
- patch realization now uses `planner_boundary_overwrite_bytes` as the overwrite length
- current verified target shape yields `15` bytes from `5,5,5`
- the trampoline copies the exact decoded span, appends an absolute jump back, and the target site is detoured with a 12-byte absolute jump plus NOP fill to the planner length

Expected next-run signals:
- `patch_write_realized=1`
- `bytes_overwritten=15`
- `trampoline_realized=1`
- if the site is executed, `detour_entry_count` should rise
