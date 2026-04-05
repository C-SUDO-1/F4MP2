# Goal 0 backend auto-step

This iteration connects the remote proxy backend tick into the live movement bridge.

## Purpose
When the scene-oriented proxy backend is used, network updates should not only update target transforms; the backend should also be advanced over time so presented transforms move toward targets.

## Current behavior
- `RemoteProxyBackend` now optionally exposes `step(user)`.
- `MovementLiveClientBridge` automatically calls `rpb_step_backend()` once per tick by default.
- Backends that do not need ticking can ignore the callback.
- The FO4 scene proxy backend stub wires `step` to `fo4_scene_proxy_backend_stub_step()`.

## Why this matters
This gets Goal 0 closer to a real in-game proxy actor model where visible state is advanced over time, not only overwritten by incoming network snapshots.
