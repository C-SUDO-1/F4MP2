# Phase 1R — Harness service dependency

## Problem

Several live movement smoke tests fork `fo4_coop_service` directly.
When developers build only the smoke-test targets, the service binary may not exist in the build tree yet.
That failure mode looks like transport or local-player-id instability even when the movement path is healthy.

## What this iteration changes

- All smoke tests that spawn the service now depend on `fo4_coop_service`.
- Those tests also receive the exact built service path through `F4MP_SERVICE_BINARY_PATH`.
- A shared test helper prints an explicit early-exit diagnostic if the service cannot start.

## Why this matters

This removes a misleading class of false negatives from Goal 0 and Phase 1R.
The remaining failures are more trustworthy:
- engine seam crash
- interpolation semantics
- true runtime/profile/hook issues
