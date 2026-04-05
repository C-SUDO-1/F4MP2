#include "phase1r_real_hook_arming_gate.h"

#include <string.h>
#include <stdio.h>

static Phase1RRealHookArmingGateState g_state;

static Phase1RRealHookArmingGateConfig default_config(void) {
    Phase1RRealHookArmingGateConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.require_candidate_activated = true;
    cfg.require_runtime_profile_supported = true;
    cfg.require_patch_policy_armed = true;
    return cfg;
}

static void append_arm_trace(void) {
    FILE* f = fopen(
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_real_arm_trace.txt",
        "a");
    if (!f) {
        return;
    }

    fprintf(
        f,
        "arm=%d activated=%d runtime=%d patch=%d evidence=%d failure=%d profile=%s site=%s\n",
        g_state.arming_allowed ? 1 : 0,
        g_state.candidate_activated ? 1 : 0,
        g_state.runtime_profile_supported ? 1 : 0,
        g_state.patch_policy_armed ? 1 : 0,
        g_state.evidence_ready ? 1 : 0,
        (int)g_state.failure,
        g_state.runtime_profile_name ? g_state.runtime_profile_name : "",
        g_state.site_prototype_name ? g_state.site_prototype_name : "");
    fclose(f);
}

void phase1r_rhag_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rhag_evaluate(const Phase1RRealHookArmingGateConfig* config) {
    Phase1RRealHookArmingGateConfig active = default_config();
    if (config) active = *config;

    phase1r_rhag_reset();
    g_state.evaluated = true;
    g_state.evidence_ready = phase1r_rhe_collect(&active.evidence_config);
    g_state.evidence_state = phase1r_rhe_state();
    g_state.candidate_state = clf4_phlcc_state();
    g_state.candidate_activated = g_state.candidate_state.activated;
    g_state.runtime_profile_supported = g_state.candidate_state.runtime_profile_supported;
    g_state.patch_policy_armed = (g_state.candidate_state.activated && g_state.candidate_state.install_ready);
    g_state.runtime_profile_name = g_state.candidate_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.candidate_state.site_prototype_name;

    if (active.require_candidate_activated && !g_state.candidate_activated) {
        g_state.failure = PHASE1R_RHAG_FAIL_CALLBACK_NOT_ACTIVATED;
    } else if (active.require_runtime_profile_supported && !g_state.runtime_profile_supported) {
        g_state.failure = PHASE1R_RHAG_FAIL_RUNTIME_PROFILE;
    } else if (active.require_patch_policy_armed && !g_state.patch_policy_armed) {
        g_state.failure = PHASE1R_RHAG_FAIL_PATCH_POLICY_NOT_ARMED;
    } else {
        g_state.failure = PHASE1R_RHAG_FAIL_NONE;
    }

    g_state.arming_allowed = (g_state.failure == PHASE1R_RHAG_FAIL_NONE);

    if (g_state.candidate_activated &&
        g_state.runtime_profile_supported &&
        g_state.patch_policy_armed) {
        g_state.arming_allowed = true;
        g_state.failure = PHASE1R_RHAG_FAIL_NONE;
    }

    append_arm_trace();
    return g_state.arming_allowed;
}

Phase1RRealHookArmingGateState phase1r_rhag_state(void) {
    return g_state;
}
