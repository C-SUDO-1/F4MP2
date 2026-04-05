# Protocol v1

## Principles
- Host authoritative
- Vanilla-first: replicate generic world, actor, combat, workshop, dialogue, and quest state rather than inventing custom multiplayer game systems
- Thin plugin, external service
- UDP transport with a small reliability layer at the message level
- Separate channels for control, world, combat, workshop, and UI mirror traffic

## Message families
- Control: hello, welcome, ping/pong, disconnect, profile snapshot, profile inventory snapshot
- profile inventory snapshot
- World: player state, interest/bubble, actor spawn/state/despawn
- Combat: fire intent, melee intent, damage result, death event
- Workshop: build request/result, object spawn/update/despawn
- UI mirror: dialogue and quest messages (later phases)

## Identity
- Stable client GUID is supplied in `HELLO`
- Service maps a GUID to a persisted guest profile path
- Session slot IDs remain transient and service-owned

## Snapshot replay
On join/reconnect, the service may replay:
- current relevant player states
- current relevant actor spawn/state
- current relevant workshop objects
- guest profile snapshot
- profile inventory snapshot

Snapshot replay is interest-filtered and should remain generic.

## Current implementation note
The scaffold currently uses simple spatial filtering for interest replay. Cell-aware Fallout 4 streaming logic is still future work.


Updated in this iteration: profile replay now includes compact perks and appearance/loadout state over MSG_PROFILE_PERKS and MSG_PROFILE_APPEARANCE.
