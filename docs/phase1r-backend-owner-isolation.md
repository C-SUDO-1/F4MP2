# Phase 1R backend owner isolation

Iteration 147 adds an explicit `owns_remote_backend` bridge config flag for same-process harness cases.

Purpose:
- prevent a sender bridge from auto-stepping a recipient-owned remote backend in two-client smoke tests
- keep manual interpolation tests honest
- reduce same-process harness-only state bleed without changing the real single-process-per-client deployment model

Default behavior remains unchanged for real runtime use: each process owns its own remote backend and `owns_remote_backend` defaults to true.
