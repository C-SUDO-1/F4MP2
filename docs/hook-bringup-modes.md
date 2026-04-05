# Hook Bring-up Modes

The plugin scaffold now models explicit bring-up modes instead of assuming every hook family is always present.

## Modes

- `HOOK_BRINGUP_NONE`
  - no hook families installed
- `HOOK_BRINGUP_BASIC_SYNC`
  - installs player + workshop families
  - enough for movement/state sync and workshop mirroring
- `HOOK_BRINGUP_VANILLA_MIRROR`
  - installs player + actor + workshop + dialogue/quest families
  - enough for the current vanilla-first mirror target

## Why this exists

This turns hook-family installation from an all-or-nothing stub into a controlled bring-up step.
It also gives startup code a way to self-check whether the requested runtime mode is actually ready.
