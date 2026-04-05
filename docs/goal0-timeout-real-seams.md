# Goal 0 timeout real-seam test

Iteration 132 proves that a remote proxy created through the newer Goal 0 seam path is removed correctly when the service times out a silent client.

Covered path:

1. host client joins through `MovementLiveClientBridge`
2. host movement enters through `clf4_phob_submit(...)`
3. guest receives the remote player through `rpm -> fo4_scene_proxy_backend_stub -> fo4_proxy_actor_driver`
4. host stops ticking entirely
5. service times the host out via `F4MP_PLAYER_TIMEOUT_TICKS`
6. guest receives `MSG_PLAYER_LEFT`
7. runtime, scene backend, and proxy driver all converge on despawned state
