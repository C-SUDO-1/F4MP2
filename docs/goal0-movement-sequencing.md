# Goal 0 Movement Sequencing

This iteration adds sequence and observed-tick fields to `MsgPlayerState` so remote proxies can reject stale or out-of-order movement updates.

## Added fields
- `state_sequence`: monotonically increasing per local movement publisher
- `observed_tick`: local observation tick when the snapshot was captured

## Current use
- Plugin movement sync bridge stamps outbound states
- Service relays the fields unchanged
- Remote proxy manager rejects stale/out-of-order states before applying smoothing/backend updates
