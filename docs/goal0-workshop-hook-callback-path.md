# Goal 0 workshop hook callback path

This iteration adds a CommonLibF4-style workshop callback stub and a bridge test.

Purpose:
- keep the engine-facing ingress model consistent across player, actor, and workshop families
- prove that a future workshop callback can feed the canonical live hook pipeline
- avoid baking synthetic-only workshop events directly into tests

Current path:
1. callback registration
2. callback emit with `WorkshopHostEvent`
3. bridge callback submits to `lhps_submit_workshop_event(...)`
4. hook dispatch translates to object spawn/update/despawn
5. engine bridge snapshot reflects the resulting object state

This is still a stub path, not a real FO4/CommonLibF4 live hook.
