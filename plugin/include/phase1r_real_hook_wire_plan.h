#pragma once

#include <stdbool.h>

#include "phase1r_real_hook_fallback_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RRealHookWireAction {
    PHASE1R_RHWP_ACTION_HOLD_SCAFFOLD = 0,
    PHASE1R_RHWP_ACTION_RUN_DRY_RUN_ONLY = 1,
    PHASE1R_RHWP_ACTION_WIRE_REAL_HOOK = 2
} Phase1RRealHookWireAction;

typedef struct Phase1RRealHookWirePlanConfig {
    Phase1RRealHookFallbackPlanConfig fallback_config;
} Phase1RRealHookWirePlanConfig;

typedef struct Phase1RRealHookWirePlanState {
    bool evaluated;
    bool wire_ready;
    Phase1RRealHookWireAction action;
    const char* rationale;
    Phase1RRealHookFallbackPlanState fallback_state;
} Phase1RRealHookWirePlanState;

void phase1r_rhwp_reset(void);
Phase1RRealHookWireAction phase1r_rhwp_plan(const Phase1RRealHookWirePlanConfig* config);
Phase1RRealHookWirePlanState phase1r_rhwp_state(void);
const char* phase1r_rhwp_action_name(Phase1RRealHookWireAction action);

#ifdef __cplusplus
}
#endif
