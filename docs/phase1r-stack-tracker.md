# Phase 1R stack tracker

Iteration 150 adds a tiny machine-readable stack snapshot for the current phase.

Current reading:
- done: Goal 0 harness join/local-player-id
- done: Goal 0 real-seams roundtrip
- done: same-process backend owner isolation
- done: manual interpolation contract in the scene backend
- current: hook prototype dry-run/runtime gates
- next: first real FO4 callback path
- blocked: first real in-game proxy actor

Purpose:
- keep the handover honest
- avoid adding new abstraction layers while the stack still says the real FO4 callback and real proxy actor are ahead of us


## Iteration 151

Promote the first engine-facing local callback candidate surface to current. Keep live FO4 callback validation blocked until a true in-engine detour is exercised.


## Iteration 152

Promote the first engine-facing proxy actor candidate surface to current. Keep live in-game FO4 actor validation blocked until a true actor creation/update path is exercised in-engine.
