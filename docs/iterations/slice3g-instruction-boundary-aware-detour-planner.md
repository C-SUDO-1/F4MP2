# Slice 3G — Instruction-Boundary-Aware Detour Planner

Purpose:
- keep the safety rollback in place
- decode the selected target prologue instruction-by-instruction
- compute a boundary-aligned overwrite length instead of assuming a fixed 12-byte cut
- record whether the captured prologue appears relocation-safe

What changed:
- the patch-plan verifier now decodes whole x64 instructions from the target prologue
- the verifier emits:
  - planned_min_jump_bytes
  - planner_decode_supported
  - planner_instruction_count
  - planner_instruction_lengths
  - planner_boundary_overwrite_bytes
  - planner_relocation_safe
- if the verifier cannot decode enough bytes safely, it reports `planner_unsupported_instruction`

Expected current target outcome:
- for the `CreatePlayerControlHandlers` manual target, the planner should show that the old 12-byte cut was not boundary-aligned
- the boundary-aware overwrite length should grow to the next whole-instruction boundary
- the slice remains verification-only and should not re-enable the live overwrite
