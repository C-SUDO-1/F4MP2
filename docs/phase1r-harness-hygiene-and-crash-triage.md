# Phase 1R Harness Hygiene and Crash Triage

Iteration 155 fixes three confirmed harness defects:

- duplicate local variable in `movement_live_scene_backend_autostep_smoke_test`
- stale runtime-profile API usage in `phase1r_toolchain_manifest_smoke_test`
- missing CTest registration for several live movement smoke tests

The remaining engine-seam roundtrip crash is still open after these hygiene fixes. This iteration intentionally leaves that crash visible so the next pass can isolate it with a reduced, trustworthy harness surface.

## Engine-seam crash root cause

The deterministic crash in `movement_live_client_engine_seams_roundtrip_smoke_test` came from `fo4_proxy_backend_stub_install()`. The stub installed a `RemoteProxyBackend` without zero-initializing the struct and without assigning `step`. When the live client bridge auto-stepped the backend, `rpb_step_backend()` could call a garbage function pointer.

Iteration 155 fixes that by zero-initializing the backend stub install struct and explicitly setting `step = 0`.
