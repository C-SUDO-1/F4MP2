# Phase 1R: hook-source candidate adapter

Iteration 139 adds the first adapter that treats the player-hook callback stub as a
hook-source candidate instead of pushing samples directly into the runtime probe.

What it does:
- installs the probe-backed local-player provider
- registers a callback with `commonlibf4_player_hook_callback_stub`
- forwards callback-shaped player-hook args into `commonlibf4_player_hook_runtime_probe`
- keeps the existing movement bridge consuming the provider path, unchanged

Why this matters:
- Iteration 138 proved that the runtime probe could act as a movement source
- It still fed the probe by calling `clf4_phrp_submit_callback(...)` directly in tests
- Iteration 139 moves one step closer to a real hook by requiring the sample to enter
  through a callback registration/emission path before the probe sees it

What it still does not prove:
- a real FO4 detour site
- a real trampoline or patch install
- a true engine callback firing from the game runtime

Acceptance for this iteration:
- callback-stub emission reaches the runtime probe through the new adapter
- the probe-backed provider captures the accepted sample
- the movement bridge emits `MsgPlayerState` from that callback-fed path
