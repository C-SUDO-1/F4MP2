# Phase 1R: player hook install candidate path

Iteration 140 adds the first single bring-up path for the local movement source candidate.

What it does:
- configures the player-hook runtime probe
- attempts player-hook installation through the address provider and install stub
- installs the callback-fed source-candidate adapter only after install succeeds
- exposes one combined state surface for:
  - install readiness
  - callback registration
  - provider readiness
  - hook correctness

Why this matters:
- Iteration 139 still required three separate manual steps in tests:
  1. configure/install the runtime probe
  2. install the source-candidate adapter
  3. emit a callback through the stub
- Iteration 140 turns the first two steps into one explicit install-candidate path
- that makes the next live FO4-backed work much easier to judge because the
  install attempt and the callback-fed local provider now belong to one seam

What it still does not prove:
- a real CommonLibF4 detour site
- a real trampoline patch landing in the game runtime
- a real engine callback firing from Fallout 4 itself

Acceptance for this iteration:
- failed install does not register the callback adapter
- successful install registers the callback-fed provider path
- one callback-stub emission reaches the movement bridge through the installed path
