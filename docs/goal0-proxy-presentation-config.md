# Goal 0 proxy presentation config

Remote presentation in Goal 0 spans three knobs:

1. `RemoteProxyUpdatePolicy`
2. `Fo4SceneProxyInterpolationConfig`
3. `MovementLiveClientBridgeConfig.remote_backend_step_count`

Recommended Goal 0 profile:

```c
RemoteProxyUpdatePolicy policy = {
    .enabled = true,
    .teleport_distance = 8.0f,
    .position_lerp_alpha = 0.25f,
    .rotation_lerp_alpha = 0.35f,
};

Fo4SceneProxyInterpolationConfig interp = {
    .enabled = true,
    .position_alpha = 0.25f,
    .rotation_alpha = 0.35f,
};

MovementLiveClientBridgeConfig live_cfg = {
    .heartbeat_interval_ticks = 10u,
    .auto_step_remote_backend = true,
    .remote_backend_step_count = 2u,
    .apply_scene_backend_interp_config = true,
    .scene_interp_config = interp,
};
```
