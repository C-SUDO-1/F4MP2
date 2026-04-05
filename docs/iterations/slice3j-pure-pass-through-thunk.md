# Slice 3J — Pure Pass-Through Thunk

Purpose: bypass the C-level detour thunk on the live path.

Implementation:
- patch realization now allocates a tiny executable gateway that immediately jumps to the realized trampoline
- the target is patched to that gateway instead of a normal C thunk
- the existing C thunk remains only as a benign fallback and performs no bookkeeping

Expected interpretation:
- crash persists: raw detour/trampoline mechanics are still wrong
- crash disappears: entering a normal C thunk was the fault line
