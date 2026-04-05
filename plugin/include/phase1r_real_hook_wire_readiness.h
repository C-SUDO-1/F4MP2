#pragma once

#include <stdbool.h>

#include "phase1r_real_hook_wire_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Phase1RRealHookWireReadinessConfig {
    Phase1RRealHookWirePlanConfig wire_plan_config;
} Phase1RRealHookWireReadinessConfig;

typedef struct Phase1RRealHookWireReadinessState {
    bool evaluated;
    bool ready_to_attempt_real_hook;
    bool evidence_ready;
    bool arming_allowed;
    bool wire_ready;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    Phase1RRealHookWireAction recommended_action;
    Phase1RRealHookWirePlanState wire_plan_state;
} Phase1RRealHookWireReadinessState;

void phase1r_rhwr_reset(void);
bool phase1r_rhwr_evaluate(const Phase1RRealHookWireReadinessConfig* config);
Phase1RRealHookWireReadinessState phase1r_rhwr_state(void);

#ifdef __cplusplus
}
#endif
