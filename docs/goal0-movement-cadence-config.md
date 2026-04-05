# Goal 0 movement cadence config

The movement cadence lives in `MovementSyncBridgeConfig`.

Recommended Goal 0 profile:

```c
MovementSyncBridgeConfig cfg = {
    .position_epsilon = 0.02f,
    .rotation_epsilon = 1.0f,
    .send_velocity = true,
    .force_resend_ticks = 20u,
    .min_send_interval_ticks = 2u,
};
```

Use stricter values only when validating a real in-engine hook and you need to inspect raw high-frequency motion.
