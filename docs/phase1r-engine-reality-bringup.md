# Phase 1R start - engine reality bringup contract

This iteration does not claim a real FO4 hook or a real in-game proxy actor yet.

It adds the two contract layers needed before those claims can be trusted:

1. `commonlibf4_player_hook_runtime_probe.*`
   - records the player-hook install attempt against the address provider
   - records whether the required addresses resolved
   - records callback acceptance, rejection, and player mismatch
   - exposes a single `hook_correct` bit that means:
     - install succeeded
     - required addresses exist
     - at least one valid callback arrived
     - the callback resolved to the expected local player

2. `fo4_proxy_actor_spawn_contract.*`
   - centralizes the first remote proxy spawn rules
   - suppresses self-spawn when configured
   - applies a default proxy base form when the incoming spawn spec does not provide one
   - becomes the path used by the scene backend before it calls the proxy driver create function

Why this matters:
- the Goal 0 scaffold already proves movement through adapter/provider/backend seams
- the next real risk is false confidence: a hook can look installed but be reading the wrong actor, and a proxy can look spawned while still using ad hoc create rules
- this iteration makes both seams measurable before replacing the stubs with live FO4 work

What it still does not prove:
- a real CommonLibF4 detour site
- a real FO4 local player pointer path
- a real in-engine actor spawn or animation driver
