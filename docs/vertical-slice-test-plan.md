# Vertical Slice Test Plan

## Objective
Validate a vanilla-first co-op slice built on generic host event mirroring.

## Required path
1. Host starts service
2. Guest joins and receives welcome
3. Guest receives profile snapshot
4. Guest movement replicates
5. Existing hostile actor state can be replayed and updated
6. Guest fires and host-authoritative damage/death replicate
7. Guest places one whitelisted workshop object
8. Guest reconnect restores profile-backed state

## Assertions
- HELLO/WELCOME/profile snapshot decode correctly
- Input state updates remote player state
- Actor spawn/state/despawn roundtrip is valid
- Damage/death broadcasts are coherent
- Build request and object lifecycle are coherent
- Reconnect snapshot replay is interest-filtered
