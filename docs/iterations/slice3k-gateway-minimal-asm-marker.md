# Slice 3K — Gateway to Minimal ASM Marker

Purpose:
- keep the stable 15-byte overwrite and realized trampoline
- replace the pure pass-through gateway with a tiny marker gateway
- record one minimal pre-trampoline side effect without entering a normal C thunk

Behavior:
- save flags and scratch registers
- increment a single global marker counter
- restore flags and scratch registers
- jump immediately to the realized trampoline

Exit criteria:
- if the game launches and the marker count rises, the raw detour/trampoline path tolerates a tiny non-C gateway side effect
- if it still crashes, the remaining fault is lower-level than any bridge logic and still inside the detour/trampoline/gateway contract
