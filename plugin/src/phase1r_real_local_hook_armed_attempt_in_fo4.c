#include "phase1r_real_local_hook_armed_attempt_in_fo4.h"

#include <string.h>

static Phase1RRealLocalHookArmedAttemptInFo4State g_state;

void phase1r_rlhaf_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rlhaf_attempt(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookArmedAttemptInFo4Config* config
) {
    Phase1RRealLocalHookArmedAttemptInFo4Config active;
    memset(&active, 0, sizeof(active));
    active.require_logs_healthy = true;
    if (config) active = *config;

    phase1r_rlhaf_reset();
    g_state.attempted = true;

    g_state.dry_run_ready = phase1r_rlhdf_run(f4se, provider, &active.dry_run_config);
    g_state.dry_run_state = phase1r_rlhdf_state();

    g_state.wiring_ready = phase1r_rlhw_attempt(f4se, provider, &active.wiring_config);
    g_state.wiring_state = phase1r_rlhw_state();

    g_state.logs_healthy = phase1r_f4selp_evaluate(&active.log_probe_config);
    g_state.log_probe_state = phase1r_f4selp_state();
    g_state.expected_plugin_seen = g_state.log_probe_state.expected_plugin_seen;
    g_state.expected_plugin_dll_name = g_state.log_probe_state.expected_plugin_dll_name;

    g_state.runtime_profile_name = g_state.dry_run_state.runtime_profile_name ?
        g_state.dry_run_state.runtime_profile_name : g_state.wiring_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.dry_run_state.site_prototype_name ?
        g_state.dry_run_state.site_prototype_name : g_state.wiring_state.site_prototype_name;

    g_state.ready_for_first_plugin_drop =
        g_state.dry_run_ready &&
        g_state.wiring_ready &&
        (!active.require_logs_healthy || g_state.logs_healthy) &&
        (!active.require_plugin_absent_for_first_drop || !g_state.expected_plugin_seen);

    g_state.armed_attempt_preconditions_met =
        g_state.dry_run_ready &&
        g_state.wiring_ready &&
        (!active.require_logs_healthy || g_state.logs_healthy) &&
        g_state.expected_plugin_seen;

    if (!g_state.dry_run_ready || !g_state.wiring_ready || (active.require_logs_healthy && !g_state.logs_healthy)) {
        g_state.recommended_action = PHASE1R_RLHA_ACTION_HOLD_SCAFFOLD;
    } else if (!g_state.expected_plugin_seen) {
        g_state.recommended_action = PHASE1R_RLHA_ACTION_BUILD_PLUGIN_DLL_DROP;
    } else {
        g_state.recommended_action = PHASE1R_RLHA_ACTION_ATTEMPT_ARMED_HOOK_NOW;
    }

    return g_state.armed_attempt_preconditions_met;
}

Phase1RRealLocalHookArmedAttemptInFo4State phase1r_rlhaf_state(void) {
    return g_state;
}

const char* phase1r_rlhaf_action_name(Phase1RRealLocalHookArmedFo4Action action) {
    switch (action) {
        case PHASE1R_RLHA_ACTION_HOLD_SCAFFOLD: return "hold_scaffold";
        case PHASE1R_RLHA_ACTION_BUILD_PLUGIN_DLL_DROP: return "build_plugin_dll_drop";
        case PHASE1R_RLHA_ACTION_ATTEMPT_ARMED_HOOK_NOW: return "attempt_armed_hook_now";
        default: return "unknown";
    }
}
