# Goal 0 proxy presentation report

Iteration 134 tunes the default remote presentation profile to bias toward stability over aggressive snap behavior.

Tuned defaults:

- `teleport_distance = 8.0`
- `position_lerp_alpha = 0.25`
- `rotation_lerp_alpha = 0.35`
- `remote_backend_step_count = 2`
- `scene_interp.position_alpha = 0.25`
- `scene_interp.rotation_alpha = 0.35`

Intent:

- avoid treating ordinary short travel as teleports
- smooth position slightly more conservatively than before
- let rotation catch up faster than position
- step the backend twice per live tick by default so the presented transform settles more quickly without removing interpolation
