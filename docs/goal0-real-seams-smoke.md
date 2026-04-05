# Goal 0 real seams smoke

Iteration 130 proves the narrowest end-to-end Goal 0 path where both sides use the newer seam work instead of older direct scaffolding.

## Covered path

Local side:
- CommonLibF4-style player hook callback shape
- player hook observer bridge
- live adapter normalization
- provider-backed observer capture
- movement sync bridge

Remote side:
- transport receive
- remote proxy manager
- scene proxy backend shim
- FO4 proxy actor driver contract

## Success conditions

The smoke test is only considered green when one live session proves all of the following in order:

1. two clients join and receive local player IDs
2. hook-fed local movement is accepted by the live adapter path
3. the remote client spawns a proxy through the scene backend
4. the proxy driver records create, move, and rotate for the same remote player
5. the runtime remote player record matches the same movement sample
6. disconnect removes the runtime player and marks both scene-backend and driver state as despawned

## Why this iteration matters

This is the first Goal 0 test that exercises the current intended seams on both sides simultaneously.

It does not prove a real game hook or a real in-engine actor yet.
It does prove that the scaffold's current handoff seams are coherent enough to carry join, move, and despawn through one vertical slice.
