# Iterations 99-101

## Iteration 99
- Added `state_sequence` and `observed_tick` to `MsgPlayerState`.
- Extended player-state packet encode/decode to carry sequencing metadata.

## Iteration 100
- `movement_sync_bridge` now stamps every outgoing player snapshot with a monotonic sequence and observed tick.
- Added accessors for the last generated sequence/tick.

## Iteration 101
- `remote_proxy_manager` now rejects stale or out-of-order remote player updates before applying smoothing/backend updates.
- Added movement sequencing smoke tests.
