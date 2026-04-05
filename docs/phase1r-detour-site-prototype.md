# Phase 1R Detour Site Prototype

Iteration 142 replaces the vague `player_update_input_pair` label with one concrete engine-facing prototype:

- prototype: `player_character_update_before_input`
- update site label: `PlayerCharacter::Update`
- input site label: `PlayerControls::Update`

This does not claim the detour is real yet. It narrows the install path so the runtime probe, install candidate, and movement emit tests all report the same engine-facing site names instead of an abstract address-pair description.
