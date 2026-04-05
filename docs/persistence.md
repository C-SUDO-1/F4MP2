# Persistence

## Host world sidecar
Host-owned multiplayer metadata should live beside the host save, not inside Fallout 4 save internals.

Example:
- `<save>.mpmeta.json`

Intended contents:
- multiplayer session metadata
- workshop mirror state relevant to multiplayer
- stable object/network IDs

## Guest profile
Guest profiles are stored by stable client GUID.

Example:
- `profiles/<player_guid>.json`

Current persisted fields:
- display name
- level
- xp
- equipped form ID
- caps
- SPECIAL stats
- compact inventory list

Future fields may add:
- inventory list
- perk list
- appearance/loadout details

## Current contract
The service loads a profile on join and sends a compact profile snapshot to the client after welcome.


Updated in this iteration: profile replay now includes compact perks and appearance/loadout state over MSG_PROFILE_PERKS and MSG_PROFILE_APPEARANCE.
