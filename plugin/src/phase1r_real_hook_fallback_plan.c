#include "phase1r_real_hook_fallback_plan.h"

#include <string.h>

static Phase1RRealHookFallbackPlanState g_state;

void phase1r_rhfp_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

Phase1RRealHookFallbackMode phase1r_rhfp_plan(const Phase1RRealHookFallbackPlanConfig* config) {
    Phase1RRealHookFallbackPlanConfig active;
    memset(&active, 0, sizeof(active));
    if (config) active = *config;

    phase1r_rhfp_reset();
    g_state.evaluated = true;
    g_state.arming_allowed = phase1r_rhag_evaluate(&active.arming_config);
    g_state.arming_state = phase1r_rhag_state();

    if (g_state.arming_allowed) {
        g_state.mode = PHASE1R_RHFP_MODE_ARMED_VALIDATION;
        g_state.ready_for_wire = true;
        g_state.blocker = 0;
    } else if (active.prefer_dry_run_when_blocked && g_state.arming_state.evidence_ready && g_state.arming_state.runtime_profile_supported) {
        g_state.mode = PHASE1R_RHFP_MODE_DRY_RUN_RESOLVE_ONLY;
        g_state.ready_for_wire = false;
        g_state.blocker = "arming gate blocked; use dry-run resolve only";
    } else {
        g_state.mode = PHASE1R_RHFP_MODE_HOLD_SCAFFOLD;
        g_state.ready_for_wire = false;
        g_state.blocker = "evidence or runtime gate incomplete";
    }
    return g_state.mode;
}

Phase1RRealHookFallbackPlanState phase1r_rhfp_state(void) { return g_state; }

const char* phase1r_rhfp_mode_name(Phase1RRealHookFallbackMode mode) {
    switch (mode) {
        case PHASE1R_RHFP_MODE_HOLD_SCAFFOLD: return "hold_scaffold";
        case PHASE1R_RHFP_MODE_DRY_RUN_RESOLVE_ONLY: return "dry_run_resolve_only";
        case PHASE1R_RHFP_MODE_ARMED_VALIDATION: return "armed_validation";
        default: return "unknown";
    }
}
