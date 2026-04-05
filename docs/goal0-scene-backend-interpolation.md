# Goal 0 Scene Backend Interpolation

This slice adds backend-side interpolation state to the FO4 scene proxy backend stub.

## Why

The remote proxy manager already decides whether a movement update should be treated as a teleport or a smoothed move. The backend now keeps:

- presented transform
- target transform
- interpolation tick count

This is closer to a future in-game actor driver where the visual actor does not necessarily snap directly to the last network update.

## Current behavior

- teleport updates set presented == target immediately
- non-teleport updates set target and leave presented for `fo4_scene_proxy_backend_stub_step()` to advance
- interpolation is simple alpha-based stepping per tick
