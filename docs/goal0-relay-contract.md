# Goal 0 relay contract

Goal 0 uses a temporary `MSG_PLAYER_STATE` relay path so two players can join and see each other move.

Guardrails added in this batch:
- service-side stale/out-of-order rejection using `state_sequence` and `observed_tick`
- movement bridge rate limiting with `min_send_interval_ticks`
- late-join replay remains allowed but stale replays cannot overwrite newer live movement

This relay path is still temporary. The long-term design remains host-authoritative input -> world update -> mirrored state.
