# Goal 0 movement cadence report

Iteration 133 tightens default local-observer send cadence for a less noisy Goal 0 baseline.

Tuned defaults:

- `position_epsilon = 0.02`
- `rotation_epsilon = 1.0`
- `min_send_interval_ticks = 2`
- `force_resend_ticks = 20`
- `send_velocity = true`

Intent:

- ignore sub-centimeter jitter
- suppress back-to-back movement sends from a single observation burst
- still emit periodic authoritative refresh packets when the player remains still
