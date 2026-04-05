# Goal 0 scene backend driver shim

Iteration 129 replaces the scene backend's purely internal bookkeeping path with a proxy-driver-backed shim.

## What changed

- the scene backend still owns target vs presented transform state for interpolation
- spawn, move, rotate, and despawn are now issued through `fo4_proxy_actor_driver.*`
- interpolation steps now re-apply the presented transform through the driver contract

## Why this matters

Goal 0 remote movement is no longer only proving that a backend-local struct updated.
It now proves that the remote backend can translate service-fed player motion into the same create/move/rotate/despawn operations a future real FO4 actor driver will consume.

## Current behavior

- spawn issues `fpad_create(...)`
- update issues `fpad_move(...)` and `fpad_rotate(...)` using the presented transform
- step replays the interpolated presented transform through the driver
- despawn issues `fpad_despawn(...)`

## Still stubbed

- no real in-engine actor creation
- no real AI package or nav behavior
- no animation graph correctness
- no real equipment attach logic

## Exit criterion for the next iteration

Iteration 130 should re-run Goal 0 with:

- local side fed by the live adapter/provider path
- remote side validated through the proxy-driver-backed scene path
