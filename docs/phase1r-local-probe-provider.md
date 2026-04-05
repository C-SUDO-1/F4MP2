# Phase 1R: local probe-backed provider

Iteration 138 adds the first provider seam that reads local player state from the
player-hook runtime probe instead of from the older synthetic local-player stub.

What this proves:
- the runtime probe is not only a diagnostic surface
- the probe's accepted callback sample can now feed the existing local observer path
- the live movement bridge can emit player state from a probe-backed source candidate

This is still intentionally limited:
- the callback still enters through synthetic test submission
- no real FO4 detour or trampoline is installed yet
- the provider only exposes the latest accepted probe sample

Acceptance for this iteration:
- probe install succeeds with required addresses present
- one accepted callback becomes capturable through `lpo_capture(...)`
- the live movement roundtrip still reaches the remote proxy backend using the
  probe-backed provider path
