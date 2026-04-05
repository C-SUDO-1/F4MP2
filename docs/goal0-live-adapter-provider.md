# Goal 0 live-adapter provider integration

Iteration 127 moves the canonical local movement path one step closer to a real engine-fed observer flow.

## What changed

The local observer provider layer can now host a dedicated live-adapter-backed provider state.
That means the project no longer needs to treat hook-fed local movement as a special bridge-only path.
Instead, the `local_player_observer_provider` seam itself can own:

- live hook sample submission
- live adapter normalization and validation
- latest-sample capture for `lpo_capture(...)`
- queue diagnostics for live provider traffic

## New path

1. future FO4/CommonLibF4 hook callback fires
2. callback forwards `CommonLibF4PlayerHookArgs`
3. `lpo_live_provider_submit(...)` normalizes through `clf4_pla_normalize(...)`
4. `lpo_capture(...)` reads the provider-backed live state
5. movement sync bridge serializes the same observer state

## Why this matters

Before this iteration, the live adapter existed, but the provider seam still behaved as if hook-fed movement lived outside the normal observer-provider path.
Now the provider seam is the primary owner of hook-fed local movement samples.

That keeps Goal 0 aligned with the eventual real implementation:
- a real hook should feed the observer provider
- the movement bridge should remain unchanged
- the service should remain unchanged

## Acceptance checkpoints

A correct Iteration 127 path should show all of the following:

- `lpo_live_provider_submit(...)` accepts known-good callback samples
- `lpo_capture(...)` returns the normalized local state
- provider live accept count increments
- provider live reject count remains zero during valid movement
- movement relay still updates the remote client without protocol changes
