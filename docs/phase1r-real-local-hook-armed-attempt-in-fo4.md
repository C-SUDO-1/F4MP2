# Phase 1R — Real Local Hook Armed Attempt in FO4

Iteration 164 converts the live-stack dry-run into an armed-attempt preflight tied to actual F4SE logs.

## Goal

Answer one narrow question:

- Is the local machine healthy enough for the first armed local-hook attempt?

Without claiming that a real FO4 detour has already been observed.

## Inputs

- runtime profile / manifest match (`1.11.191`)
- F4SE build match (`0.7.7`)
- dry-run address resolution through the existing probe path
- armed mock-path wiring through the existing callback candidate path
- F4SE loader/runtime log evidence

## Log-derived evidence

The loader/runtime logs are treated as evidence of environment health, not evidence of our plugin hook working.

Healthy evidence includes:

- loader selects `f4se_1_11_191.dll`
- loader signatures match
- runtime initializes successfully
- plugin directory is scanned
- runtime reaches `init complete`

## Decision boundary

There are now two distinct states:

1. **ready_for_first_plugin_drop**
   - environment healthy
   - runtime profile matches
   - dry-run and armed mock path both pass
   - our plugin DLL is not yet seen in the runtime log

2. **armed_attempt_preconditions_met**
   - same as above
   - plus the expected plugin DLL is observed in the scanned plugin list

For the current machine logs, the correct recommendation is expected to be:

- `build_plugin_dll_drop`

not `attempt_armed_hook_now`.
