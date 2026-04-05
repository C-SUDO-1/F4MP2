# FO4 Co-op Scaffold

A vanilla-first, host-authoritative Fallout 4 co-op scaffold.

## Design goals
- Mirror existing Fallout 4 gameplay systems instead of inventing custom multiplayer-only game modes.
- Keep the in-engine plugin thin and push session, authority, persistence, and replication into an external service.
- Treat the host as the canonical owner of world state, combat truth, dialogue progression, and quest progression.
- Keep the initial playable scope narrow: player presence, hostile actor sync, host-authoritative combat, workshop sync, and guest profile persistence.

## Current scope
- Thin F4SE/plugin contract layer with protocol apply stubs
- External authoritative service
- Session and reconnect identity by stable client GUID
- Join snapshot replay for current relevant world state
- Player replication
- Generic actor lifecycle replication
- Host-authoritative combat
- Workshop whitelist + build/move/scrap flow
- Guest profile persistence
- Dialogue mirror scaffolding
- Quest mirror scaffolding
- Live UDP harness and smoke tests

## Explicit non-goals for this scaffold
- No custom settlement wave game mode
- No split-party roaming
- No full quest sync yet
- No full dialogue cinematics yet
- No power-grid sync yet
- No mod compatibility layer yet

## Repository layout
- `shared/` protocol, schemas, and common types
- `service/` host-authoritative multiplayer service
- `plugin/` thin plugin/runtime contracts and protocol apply stubs
- `docs/` architecture, protocol, persistence, and test-plan docs

## Build
```bash
cmake -S . -B build
cmake --build build
```

## Main binaries
- `fo4_coop_service`
- `udp_client_harness`
- `integration_smoke_test`
- `codec_roundtrip_test`
- `plugin_contract_smoke_test`
- `plugin_apply_smoke_test`
- `f4mp_f4se_plugin.dll` (windows-only scaffold export target; copied after build into `Data/F4SE/Plugins/`)

## Notes
- Workshop form IDs in the whitelist are still placeholders and must be replaced with real Fallout 4 workshop form IDs.
- The plugin runtime is still a scaffold, but the dev live-transport path can now be enabled through `f4mp_live.ini` and will auto-open during plugin load when the flag is enabled.
- The player hook callback stub now advances the dev live-transport client once per callback, which is enough to drive handshake/polling in the current scaffold.


Updated in this iteration: profile replay now includes compact perks and appearance/loadout state over MSG_PROFILE_PERKS and MSG_PROFILE_APPEARANCE.


Updated in this iteration: vanilla-first dialogue and quest mirror protocol/apply scaffolding, plus service-side host-event broadcast helpers.


## Current focus
- vanilla-first co-op architecture
- thin FO4/F4SE bridge direction
- host-event mirroring for dialogue and quest UI state
- no custom gameplay subsystem in the core protocol

## New docs
- `docs/fo4-hook-alignment.md`
- `docs/existing-mp-structure-review.md`

## Upstream plugin direction

The long-term engine-facing plugin path is aligned to current CommonLibF4/CommonLibF4-template style development with F4SE runtime still required. The scaffold keeps that layer thin and pushes multiplayer authority/persistence/session logic outside the DLL.



## Latest iteration focus

- optional `real_plugin_skeleton` target for future CommonLibF4/F4SE DLL work
- concrete FO4-system-oriented hook shim groups feeding the ingress layer
- quest mirror policy validation on the service side


## Recent additions
- Runtime workshop whitelist loading from JSON
- Hook install registry for future CommonLibF4/F4SE hook-family bring-up


Recent additions:
- CommonLibF4-style adapter now covers player, actor, workshop, dialogue, and quest hook-family submission through the canonical live hook pipeline.
- Hook bring-up policy reports when installed hook families are sufficient for basic sync vs full vanilla mirror.
- Workshop whitelist can now be imported from TSV into runtime JSON for replacing placeholder form IDs without code edits.


Latest iteration highlights: dialogue/quest family-specific hook attempt stubs, structured startup report accessor, and a CommonLibF4 address-provider seam used by the player hook attempt path.

- Iterations 69-71: provider-backed actor/workshop attempts and bring-up report accessor.


Latest iteration notes: provider-backed hook attempts now cover all modeled families, and hook attempt results include address-resolution detail for deeper diagnostics.


## Iteration 89
- added `MSG_PLAYER_LEFT` and live disconnect/despawn roundtrip coverage for Goal 0.


## Iteration 92
- added ping/pong heartbeat support
- added service-side timeout disconnect path
- added keepalive live roundtrip coverage for Goal 0 movement


## Dev live transport config
- Copy `f4mp_live.ini.example` to `f4mp_live.ini`, `F4SE/f4mp_live.ini`, or `Data/F4SE/Plugins/f4mp_live.ini`.
- Set `dev_live_transport = 1` to opt in.
- Advanced users can override the file location with the `F4MP_LIVE_CONFIG` environment variable.
- On Windows, build `f4mp_f4se_plugin` to emit `f4mp_f4se_plugin.dll`; the build now also copies the DLL into `Data/F4SE/Plugins/` inside the repo for staging.
- The DLL exports scaffold entrypoints `F4SEPlugin_Query` and `F4SEPlugin_Load`, plus debug helpers `F4MP_LiveTick`, `F4MP_LiveEnabled`, `F4MP_LiveConnected`, and `F4MP_LiveConfigPath`.
