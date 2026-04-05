#include "phase1r_real_hook_wire_plan.h"

#include <string.h>

static Phase1RRealHookWirePlanState g_state;

void phase1r_rhwp_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

Phase1RRealHookWireAction phase1r_rhwp_plan(const Phase1RRealHookWirePlanConfig* config) {
    Phase1RRealHookWirePlanConfig active;
    memset(&active, 0, sizeof(active));
    if (config) active = *config;

    phase1r_rhwp_reset();
    g_state.evaluated = true;
    phase1r_rhfp_plan(&active.fallback_config);
    g_state.fallback_state = phase1r_rhfp_state();

    switch (g_state.fallback_state.mode) {
        case PHASE1R_RHFP_MODE_ARMED_VALIDATION:
            g_state.action = PHASE1R_RHWP_ACTION_WIRE_REAL_HOOK;
            g_state.wire_ready = true;
            g_state.rationale = "armed validation path is green";
            break;
        case PHASE1R_RHFP_MODE_DRY_RUN_RESOLVE_ONLY:
            g_state.action = PHASE1R_RHWP_ACTION_RUN_DRY_RUN_ONLY;
            g_state.wire_ready = false;
            g_state.rationale = "arming blocked; resolve addresses only";
            break;
        case PHASE1R_RHFP_MODE_HOLD_SCAFFOLD:
        default:
            g_state.action = PHASE1R_RHWP_ACTION_HOLD_SCAFFOLD;
            g_state.wire_ready = false;
            g_state.rationale = "scaffold still missing evidence or runtime support";
            break;
    }

    return g_state.action;
}

Phase1RRealHookWirePlanState phase1r_rhwp_state(void) {
    return g_state;
}

const char* phase1r_rhwp_action_name(Phase1RRealHookWireAction action) {
    switch (action) {
        case PHASE1R_RHWP_ACTION_HOLD_SCAFFOLD: return "hold_scaffold";
        case PHASE1R_RHWP_ACTION_RUN_DRY_RUN_ONLY: return "run_dry_run_only";
        case PHASE1R_RHWP_ACTION_WIRE_REAL_HOOK: return "wire_real_hook";
        default: return "unknown";
    }
}
