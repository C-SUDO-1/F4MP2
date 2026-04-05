# Phase 1R — live in-game proxy validation

Iteration 154 adds the first explicit validation layer for the live proxy actor candidate path.

This layer does not claim a real in-engine actor exists yet. It verifies that the current
engine-facing proxy candidate seam is internally coherent in the scaffold harness:

- live candidate enabled
- scene backend player exists and is visible
- driver state exists and is visible
- scene actor handle matches driver actor handle
- scene presented transform matches driver transform
- proxy base form id is present
- candidate has recorded spawn/move/rotate activity
- despawn path marks both scene and driver state as despawned

This makes the proxy side diagnosable in the same way Iteration 153 made the callback side
diagnosable.
