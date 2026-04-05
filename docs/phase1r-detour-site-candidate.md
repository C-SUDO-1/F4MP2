# Phase 1R: player detour-site candidate descriptor

Iteration 141 adds one explicit detour-site candidate descriptor and wires it into the
player hook install-candidate path.

What it does:
- names the current player hook-site assumption as `player_update_input_pair`
- captures the required address keys for that candidate
- captures the minimum supported runtime for that candidate
- resolves and reports the candidate through the runtime probe and install-candidate stats

Why this matters:
- Iteration 140 still treated the local movement hook candidate as an unnamed pair of
  addresses
- Iteration 141 makes the assumed hook site explicit, inspectable, and testable
- that is the minimum contract needed before trying a real FO4 detour site in-engine

What it still does not prove:
- that the named candidate is the correct real Fallout 4 detour site
- that a trampoline patch is landing in the engine
- that the engine callback cadence is correct in live play

Acceptance for this iteration:
- the install path reports which candidate was used
- candidate runtime/address resolution is visible in probe/install stats
- Goal 0 movement still works through the same installed local path
