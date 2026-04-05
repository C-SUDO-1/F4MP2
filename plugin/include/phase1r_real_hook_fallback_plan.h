#pragma once

#include <stdbool.h>

#include "phase1r_real_hook_arming_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RRealHookFallbackMode {
    PHASE1R_RHFP_MODE_HOLD_SCAFFOLD = 0,
    PHASE1R_RHFP_MODE_DRY_RUN_RESOLVE_ONLY = 1,
    PHASE1R_RHFP_MODE_ARMED_VALIDATION = 2
} Phase1RRealHookFallbackMode;

typedef struct Phase1RRealHookFallbackPlanConfig {
    Phase1RRealHookArmingGateConfig arming_config;
    bool prefer_dry_run_when_blocked;
} Phase1RRealHookFallbackPlanConfig;

typedef struct Phase1RRealHookFallbackPlanState {
    bool evaluated;
    bool ready_for_wire;
    bool arming_allowed;
    Phase1RRealHookFallbackMode mode;
    const char* blocker;
    Phase1RRealHookArmingGateState arming_state;
} Phase1RRealHookFallbackPlanState;

void phase1r_rhfp_reset(void);
Phase1RRealHookFallbackMode phase1r_rhfp_plan(const Phase1RRealHookFallbackPlanConfig* config);
Phase1RRealHookFallbackPlanState phase1r_rhfp_state(void);
const char* phase1r_rhfp_mode_name(Phase1RRealHookFallbackMode mode);

#ifdef __cplusplus
}
#endif
