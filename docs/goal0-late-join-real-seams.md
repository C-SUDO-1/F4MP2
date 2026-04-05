# Goal 0 late join through real seams

Iteration 131 proves the late-join replay path still works after the Goal 0 seam replacements.

## What is being proven

A player who joins after an earlier movement update has already been accepted by the service still receives the replayed remote player correctly through:

- local hook-fed live adapter/provider path on the moving player
- remote proxy manager path on the late joiner
- scene-backend-to-proxy-driver path on the late joiner

## Smoke test

`movement_late_join_real_seams_smoke_test`

The test performs this sequence:

1. connect player A through `MovementLiveClientBridge`
2. submit one hook-shaped local movement sample through `clf4_phob_submit(...)`
3. tick until the movement bridge sends the player state to the service
4. connect player B later through `PluginTransportClient`
5. install `rpm` with `fo4_scene_proxy_backend_stub_install()` on B
6. verify the replayed `MSG_PLAYER_STATE` creates and updates the remote proxy through the scene backend and proxy driver

## Acceptance criteria

- the moving player sample is accepted by the live adapter path
- late join replay creates a remote runtime player on B
- late join replay creates a scene-backend proxy record on B
- late join replay creates a proxy-driver actor record on B
- replayed transform, stance, and animation state match the earlier movement sample
