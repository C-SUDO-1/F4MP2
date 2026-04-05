# Phase 1R unresolved-family probe and harvest slices

This batch changes the unresolved-family behavior from:

- unresolved -> reject -> stop

into:

- unresolved -> probe_only session -> harvest reachable seams -> cluster -> additive promotion traces

## What it adds

- `player_hook_probe_harvest.*`
- probe session metadata on `HookDiscoveryReport`
- `f4mp_probe_harvest_trace.txt`
- `f4mp_callsite_harvest_trace.txt`
- `f4mp_candidate_cluster_trace.txt`
- `f4mp_discovered_candidate_promotion_trace.txt`
- additive discovered-candidate naming for unresolved `PlayerCharacter::Update` families

## Current intent

This does not replace the authored candidate definitions.
It creates discovery-side evidence and additive promotion traces so unresolved families can move forward without manual RVAs.

## Reference influence

- Addictol: runtime-gated hook/patch patterns and controls-path relevance
- AddictolCrashLogger: diagnostic-first workbench posture and symbol-friendly tracing
- commonlibf4/commonlib-shared: engine-type and REL-layer surface area
