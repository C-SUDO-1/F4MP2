# Goal 0 player live adapter

Iteration 126 introduces `commonlibf4_player_live_adapter.*` as the canonical normalization seam between a future CommonLibF4/F4SE player hook callback and the existing local observer bridge.

## Purpose

Before this iteration, `clf4_phob_submit(...)` performed direct field copying from `CommonLibF4PlayerHookArgs` into `LocalPlayerObservedState`.
That was sufficient for a stub path, but it left no single place to define what a *usable* live player sample actually is.

The live adapter now owns:

- hook-callback shaped input
- fallback local player ID resolution
- finite-value validation for position / rotation / velocity
- stance validation against the current protocol enum
- accepted-sample cadence
- rejection diagnostics

## Contract

Input:
- `CommonLibF4PlayerHookArgs`

Output:
- `CommonLibF4PlayerLiveSample`
  - normalized `LocalPlayerObservedState`
  - `observed_tick`
  - `accepted_sequence`
  - `source_kind`
  - whether fallback local player ID was used

## Acceptance rules

A sample is accepted only when all of the following are true:

- callback args pointer is non-null
- output pointer is non-null
- player ID resolves either from the event or the configured fallback local player ID
- position components are finite
- rotation components are finite
- velocity components are finite
- stance is within the current protocol range `STANCE_IDLE..STANCE_FALL`

## Cadence rules

The adapter is queue-friendly by design.

- every accepted normalized sample increments `observed_tick`
- every accepted normalized sample increments `accepted_sequence`
- rejected samples do **not** advance cadence
- the observer bridge remains free to keep only the latest queued sample while preserving adapter-side cadence diagnostics

This keeps the canonical contract compatible with the current queue-backed observer bridge and later real hook-fed provider work.

## How to know the hook is working correctly

At the scaffold level, a correctly wired hook path should satisfy all of the following:

1. the callback stub emits successfully
2. the live adapter `accept_count` increments
3. the live adapter `reject_count` remains zero during known-good movement
4. the observer bridge `submit_count` and `capture_count` increment
5. `lpo_capture(...)` returns the same transform that was observed locally
6. end-to-end movement relay produces the same remote proxy update on the other client

A future real hook implementation should preserve those exact checkpoints.
