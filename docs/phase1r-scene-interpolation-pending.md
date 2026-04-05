# Phase 1R scene interpolation pending

Iteration 148 makes interpolation state explicit in the scene backend.

Added field:
- `interpolation_pending` on `Fo4SceneProxyPlayerState`

Semantics:
- set true when the backend has a new target transform but presented state has not yet converged
- cleared when interpolation is disabled, when teleport policy snaps directly to target, or when presented and target become equal

This makes manual-step tests assert the right thing directly instead of inferring it indirectly from position deltas.
