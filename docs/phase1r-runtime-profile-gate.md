# Phase 1R runtime profile gate

Iteration 144 adds a runtime-profile check in front of the current
`player_character_update_before_input` prototype.

The current profile is explicit and exact-match only:
- profile: `fo4_1_10_984_player_character_update_before_input`
- required runtime: `0x010A03D8`

Dry-run and armed install now both fail fast on unsupported build signatures
before address resolution and before the callback-fed adapter path can arm.
