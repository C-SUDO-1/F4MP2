#include "phase1r_real_hook_wire_readiness.h"

#include <string.h>

static Phase1RRealHookWireReadinessState g_state;

void phase1r_rhwr_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rhwr_evaluate(const Phase1RRealHookWireReadinessConfig* config) {
    Phase1RRealHookWireReadinessConfig active;
    memset(&active, 0, sizeof(active));
    if (config) active = *config;

    phase1r_rhwr_reset();
    g_state.evaluated = true;
    phase1r_rhwp_plan(&active.wire_plan_config);
    g_state.wire_plan_state = phase1r_rhwp_state();
    g_state.evidence_ready = g_state.wire_plan_state.fallback_state.arming_state.evidence_ready;
    g_state.arming_allowed = g_state.wire_plan_state.fallback_state.arming_allowed;
    g_state.wire_ready = g_state.wire_plan_state.wire_ready;
    g_state.runtime_profile_name = g_state.wire_plan_state.fallback_state.arming_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.wire_plan_state.fallback_state.arming_state.site_prototype_name;
    g_state.recommended_action = g_state.wire_plan_state.action;
    g_state.ready_to_attempt_real_hook = g_state.wire_ready && g_state.recommended_action == PHASE1R_RHWP_ACTION_WIRE_REAL_HOOK;
    return g_state.ready_to_attempt_real_hook;
}

Phase1RRealHookWireReadinessState phase1r_rhwr_state(void) {
    return g_state;
}
