# Phase 1R: live callback candidate

Iteration 151 makes the first engine-facing local callback candidate explicit without claiming a live FO4 detour.

## What it adds

- a named callback source symbol: `PlayerCharacter::Update`
- a named callback guard symbol: `PlayerControls::Update`
- one wrapper over the install-candidate path that only activates when:
  - the supported runtime profile matches
  - the install candidate path is ready
  - the probe-backed provider is installed
  - the callback path is registered

## What it does not claim

- a real FO4 trampoline
- a verified in-game callback site
- live crash-safe validation inside FO4.exe
