# Hook install outcomes

This iteration extends hook-family tracking from a binary installed/not-installed flag into a richer state model.

## Per-family states
- `HOOK_INSTALL_NOT_ATTEMPTED`
- `HOOK_INSTALL_INSTALLED`
- `HOOK_INSTALL_FAILED`
- `HOOK_INSTALL_PARTIAL`

## Per-family errors
- unsupported runtime
- address missing
- patch rejected
- dependency missing
- unknown

## Why this matters
A real CommonLibF4/F4SE bring-up path usually fails in family-specific ways. Tracking only "installed" hides whether a family:
- was never attempted,
- partially came up,
- or failed for a reason that should block load.

This scaffold now models those outcomes so startup/readiness can be graded:
- unavailable
- partial
- ready
