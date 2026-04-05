# Slice 3F — Safe Patch Plan Verifier

Purpose:
- disable the live overwrite after the launch crash
- keep discovery/manual target selection intact
- capture target prologue and page metadata for the selected manual patch target

Behavior:
- `clf4_phpr_attempt(...)` no longer writes to the target function in this slice
- when the target and detour destination are otherwise valid, the module captures the first 16 bytes,
  queries page metadata with `VirtualQuery`, writes `f4mp_patch_plan_verify_trace.txt`, and returns
  `verification_only_safety_rollback`
- patch realization stays false (`patch_write_realized=0`, `trampoline_realized=0`, `bytes_overwritten=0`)

Exit criteria:
- no launch crash from the manual target patch path
- discovery still selects the manual controls candidate
- patch traces show `verification_only_safety_rollback`
- verifier trace contains target address, detour destination, first bytes, and page metadata
