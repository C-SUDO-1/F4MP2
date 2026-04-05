# Goal 0 heartbeat and timeout slice

This slice adds basic liveness handling to the temporary Goal 0 movement path.

## Added behavior
- plugin-side keepalive pings through the live movement bridge
- service-side ping -> pong handling
- service-side idle timeout tracking per connected player
- timeout-driven `MSG_PLAYER_LEFT` broadcast and session removal

## Notes
- timeout handling is disabled by default in `tl_init()` and must be enabled explicitly with `tl_set_player_timeout_ticks()`
- this avoids changing existing service behavior while the heartbeat slice is still a scaffold feature
- the long-term authority model will replace direct `PLAYER_STATE` relay, but the liveness path remains useful
