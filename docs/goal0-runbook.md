# Goal 0 runbook

## Build

```bash
cmake -S . -B build
cmake --build build --target fo4_coop_service movement_goal0_real_seams_smoke_test movement_late_join_real_seams_smoke_test movement_timeout_real_seams_smoke_test
```

## Run the core Goal 0 acceptance slice

```bash
./tools/run_goal0_smokes.sh ./build-goal0
```

## Manual service launch

```bash
./build/fo4_coop_service 7794
```

Optional timeout tuning:

```bash
F4MP_PLAYER_TIMEOUT_TICKS=5 ./build/fo4_coop_service 7802
```
