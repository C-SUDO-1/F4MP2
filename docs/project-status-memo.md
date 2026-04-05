# Short project status memo

## Current state
The scaffold is now a coherent vanilla-first FO4 co-op architecture, not a prototype pile.

## Working areas
- shared binary protocol and codec
- authoritative service core for sessions, combat, workshop, profile replay
- plugin/runtime apply path for player, actor, object, profile, dialogue, and quest state
- hook-facing translation seams for player, actor, workshop, and dialogue/quest events
- optional plugin entrypoint/export skeleton for a future CommonLibF4/F4SE target

## Biggest strengths
- clean separation between engine-facing plugin work and external multiplayer authority
- vanilla-first mirroring instead of custom game-mode logic
- strong smoke-test coverage across codec, service policy, plugin apply, and hook pipeline scaffolding

## Biggest gaps
- no true CommonLibF4/F4SE-linked hooks yet
- no real LOS/collision world trace implementation yet
- workshop whitelist still uses placeholder FO4 form IDs
- dialogue progression is still conservative mirrored progression, not true host-driven FO4 advancement

## Recommended next focus
1. first true hook-backed seam using the current dispatch pipeline
2. real world trace provider contract implementation for LOS/collision
3. replace placeholder workshop forms with extracted FO4 forms
