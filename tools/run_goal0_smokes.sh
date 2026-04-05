#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-$ROOT_DIR/build-goal0}"
mkdir -p "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" >/dev/null
cmake --build "$BUILD_DIR" --target \
  fo4_coop_service \
  movement_goal0_real_seams_smoke_test \
  movement_late_join_real_seams_smoke_test \
  movement_timeout_real_seams_smoke_test \
  movement_sync_bridge_cadence_profile_smoke_test \
  fo4_scene_proxy_backend_presentation_tuning_smoke_test >/dev/null
(
  cd "$BUILD_DIR"
  ./movement_goal0_real_seams_smoke_test
  ./movement_late_join_real_seams_smoke_test
  ./movement_timeout_real_seams_smoke_test
  ./movement_sync_bridge_cadence_profile_smoke_test
  ./fo4_scene_proxy_backend_presentation_tuning_smoke_test
)
