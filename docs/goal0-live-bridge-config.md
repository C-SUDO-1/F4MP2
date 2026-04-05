# Goal 0 live bridge config

This iteration exposes movement-live-bridge configuration so the live path can control
remote backend stepping behavior explicitly.

Added config fields:
- heartbeat_interval_ticks
- auto_step_remote_backend
- remote_backend_step_count

This allows the scene/proxy backend interpolation path to be exercised more realistically
without changing the service or protocol layer.
