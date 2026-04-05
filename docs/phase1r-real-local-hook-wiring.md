# Phase 1R real local hook wiring

Iteration 162 adds the first **real local hook wiring pass** over the existing scaffold stack.

It does not claim that a live Fallout 4 detour is installed. Instead, it wires the current pieces together as one explicit action:

1. activate the live callback candidate through the install-candidate path
2. emit one validation callback sample
3. validate provider-captured local state
4. re-evaluate the real-hook wire-readiness bundle after activation
5. build one `MsgPlayerState` from that wired path

The intent is to stop treating these as independent green boxes. This iteration makes them one coherent wiring attempt and records whether that attempt is strong enough to justify the first real FO4/F4SE dry run.
