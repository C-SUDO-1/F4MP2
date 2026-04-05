# Trusted address resolution dry-run slice

## Goal
Inject one trusted nonzero address source into the existing loader-stable baseline,
carry it through discovery as a resolved candidate, and forbid any real patch write.

## What changed
- Added a new address source tier: `trusted_profile`.
- Added a file-backed trusted address reader in `plugin/src/f4se_plugin_exports.cpp`.
- Added richer trusted-source fields to:
  - `f4mp_update_resolution_trace.txt`
  - `f4mp_manual_pivot_trace.txt`
- Forced the live-candidate wiring path to dry-run policy:
  - `patch_allowed = false`
  - `patch_policy = dry_run_resolve_only`
- Added `f4mp_trusted_addresses.ini.example` as the seed format.

## Trusted profile format
Expected keys:
- `runtime_version`
- `pivot`
- `update_addr` or `update_rva`
- `input_addr` or `input_rva`
- `actor_addr` or `actor_rva`

Default lookup path:
`Data/F4SE/Plugins/f4mp_trusted_addresses.ini`

Override path via env:
`F4MP_TRUSTED_ADDRESS_FILE`

## Acceptance criteria for this slice
- Plugin still loads through F4SE.
- Trusted source can populate a nonzero address for the chosen pivot.
- Discovery can see the source as resolved instead of unresolved.
- Patch-target audit can show a nonzero effective patch target.
- No real patch write is attempted.
