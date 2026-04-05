# Goal 0 Hook-Fed Provider and Scene Backend

This slice moves Goal 0 closer to the real engine boundary without claiming true
FO4/CommonLibF4 integration yet.

It adds:
- a CommonLibF4-style player-hook observer bridge that installs through the
  existing local-player observer provider seam
- a scene-oriented proxy backend stub that looks more like a future in-game
  actor driver than the simpler count-only backend
- a live roundtrip smoke test that uses those two seams together

The intended next replacement steps are:
1. swap the observer bridge submission path for true hook callbacks
2. swap the scene backend stub for real in-game actor spawn/update/despawn code
