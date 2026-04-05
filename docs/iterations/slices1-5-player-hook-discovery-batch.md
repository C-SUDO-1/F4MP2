# Slices 1-5 Player Hook Discovery Batch

This batch consolidates the player-hook discovery ladder into one coherent source state.

## Slice 1 - exact/manual resolution backend

Added:
- layered address provider support
- manual exact-address override ingestion from environment variables
- provider describe path now preserves source tags through overrides

Key APIs:
- `clf4_address_provider_make_layered(...)`
- `clf4_address_provider_read_env_overrides(...)`
- `clf4_address_provider_env_name(...)`

## Slice 2 - first real candidate injection

Discovery no longer hard-rejects the `player_controls_update` path as weak-confidence-only when it has a real executable manual/pattern/address-library source.

Outcome:
- unresolved before/after-input families can coexist with an armable controls-path candidate
- discovery can select a non-stub real candidate without rewriting discovery logic

## Slice 3 - real entry proof plumbing

Promoted real-entry state into the live-candidate surface.

Added state:
- `executing`
- `real_entry_count`
- `detour_entry_count`
- `detour_bridge_forward_count`

This makes the live candidate distinguish:
- installed but dead
- installed and executing

## Slice 4 - real callback proof plumbing

Extended validation config to distinguish synthetic callback success from real callback success.

Added validation knobs:
- `require_real_entry`
- `require_real_callback_accept`
- `min_real_callback_forward_count`
- `min_real_callback_accept_count`

Added failures:
- `CLF4_PHLV_FAIL_NO_REAL_ENTRY`
- `CLF4_PHLV_FAIL_NO_REAL_CALLBACK`

## Slice 5 - candidate sequencing

Discovery now accepts sequencing input so the module can move to the next candidate when one is known-dead.

Added:
- `HookDiscoverySequencingInput`
- candidate rank / sequence status / truth state in the discovery report
- fallback away from previously-dead candidates

Current policy order:
1. `player_character_update_before_input`
2. `player_character_update_after_input`
3. `player_controls_update`
4. `actor_update_player_filtered`

## Smoke coverage added in this batch

- `commonlibf4_address_provider_layered_smoke_test`
- `player_hook_discovery_sequence_smoke_test`
- `player_hook_real_entry_and_callback_smoke_test`

Compatibility note:
- existing live-candidate/live-validation smoke tests were adjusted to stay portable when full live patch realization is unavailable in the current build environment.
