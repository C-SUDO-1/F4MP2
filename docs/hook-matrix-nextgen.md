# Hook Matrix (Next-Gen / CommonLibF4-aligned)

This matrix maps likely Fallout 4 ingress categories to the scaffold's current bridge API.

| Vanilla source category | Bridge seam | Current scaffold target | Notes |
|---|---|---|---|
| Player update / movement | CommonLibF4 observer or equivalent hook shim | `fbi_ingest_remote_player_state` | Keep extraction thin; no authority logic here. |
| NPC combat / actor lifecycle | Actor observer shim | `fbi_ingest_actor_spawn`, `fbi_ingest_actor_state`, `fbi_ingest_death_event` | Host remains combat authority. |
| Workshop mode place/move/scrap | Workshop event shim | `fbi_ingest_object_spawn`, `fbi_ingest_object_update`, `fbi_ingest_object_despawn` | Validate in service, not in plugin. |
| Dialogue open/line/choices/close | Dialogue/UI bridge | `fbi_ingest_dialogue_open`, `fbi_ingest_dialogue_line`, `fbi_ingest_dialogue_choices`, `fbi_ingest_dialogue_close` | Mirror UI state only. |
| Guest dialogue selection | Papyrus/native callback or menu callback bridge | service `dialogue_selection_policy` ingress | Guest requests; host validates and advances. |
| Quest/objective change | Quest observer | `fbi_ingest_quest_update` | Compact mirror only. |
| Profile bootstrap / scripted native tests | Papyrus quest-script bridge | future ingress shims | Useful for controlled plugin bring-up and diagnostics. |

## Immediate next FO4-facing seam

Replace the pure stub path with:
- CommonLibF4/F4SE plugin bootstrap
- a narrow set of observer/callback shims that call the existing `fbi_*` ingress functions

The existing ingress layer is already shaped for that transition.

## Iterations 27-29 additions

- Added concrete shim groups mirroring likely vanilla FO4 event sources:
  - player hook shim
  - actor/combat hook shim
  - workshop hook shim
  - dialogue/quest hook shim
- Added optional `real_plugin_skeleton` build target to make the future CommonLibF4/F4SE DLL boundary explicit.
- Added quest mirror policy validation so mirrored quest updates are no longer accepted as arbitrary payloads.
