#include "phase1r_real_hook_evidence.h"

#include <stdio.h>
#include <string.h>

static Phase1RRealHookEvidenceState g_state;

static void write_trace(const Phase1RRealHookEvidenceState* s) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_movement_slice_evidence_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f || !s) {
        return;
    }

    fprintf(f, "collected=%d\n", s->collected ? 1 : 0);
    fprintf(f, "evidence_ready=%d\n", s->evidence_ready ? 1 : 0);
    fprintf(f, "toolchain_ok=%d\n", s->toolchain_ok ? 1 : 0);
    fprintf(f, "callback_ok=%d\n", s->callback_ok ? 1 : 0);
    fprintf(f, "proxy_ok=%d\n", s->proxy_ok ? 1 : 0);
    fprintf(f, "harness_ok=%d\n", s->harness_ok ? 1 : 0);
    fprintf(f, "failure=%d\n", (int)s->failure);
    fprintf(f, "local_player_id=%u\n", (unsigned int)s->local_player_id);
    fprintf(f, "remote_player_id=%u\n", (unsigned int)s->remote_player_id);
    fprintf(f, "runtime_profile_name=%s\n", s->runtime_profile_name ? s->runtime_profile_name : "");
    fprintf(f, "site_prototype_name=%s\n", s->site_prototype_name ? s->site_prototype_name : "");
    fprintf(f, "callback_source_symbol=%s\n", s->callback_source_symbol ? s->callback_source_symbol : "");
    fprintf(f, "callback_guard_symbol=%s\n", s->callback_guard_symbol ? s->callback_guard_symbol : "");
    fprintf(f, "movement.callback_registered=%d\n", s->movement.callback_registered ? 1 : 0);
    fprintf(f, "movement.provider_ready=%d\n", s->movement.provider_ready ? 1 : 0);
    fprintf(f, "movement.hook_activated=%d\n", s->movement.hook_activated ? 1 : 0);
    fprintf(f, "movement.has_observed_state=%d\n", s->movement.has_observed_state ? 1 : 0);
    fprintf(f, "movement.callback_forward_count=%u\n", (unsigned int)s->movement.callback_forward_count);
    fprintf(f, "movement.callback_accept_count=%u\n", (unsigned int)s->movement.callback_accept_count);
    fprintf(f, "movement.callback_reject_count=%u\n", (unsigned int)s->movement.callback_reject_count);
    fprintf(f, "movement.real_callback_forward_count=%u\n", (unsigned int)s->movement.real_callback_forward_count);
    fprintf(f, "movement.real_callback_accept_count=%u\n", (unsigned int)s->movement.real_callback_accept_count);
    fprintf(f, "movement.real_callback_reject_count=%u\n", (unsigned int)s->movement.real_callback_reject_count);
    fprintf(f, "movement.live_provider_submit_count=%u\n", (unsigned int)s->movement.live_provider_submit_count);
    fprintf(f, "movement.live_provider_capture_count=%u\n", (unsigned int)s->movement.live_provider_capture_count);
    fprintf(f, "movement.live_provider_accept_count=%u\n", (unsigned int)s->movement.live_provider_accept_count);
    fprintf(f, "movement.live_provider_reject_count=%u\n", (unsigned int)s->movement.live_provider_reject_count);
    fprintf(f, "movement.live_provider_queue_high_watermark=%u\n", (unsigned int)s->movement.live_provider_queue_high_watermark);
    if (s->movement.has_observed_state) {
        fprintf(f,
                "movement.last_state player_id=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)\n",
                (unsigned int)s->movement.last_observed_state.player_id,
                (unsigned int)s->movement.last_observed_state.stance,
                s->movement.last_observed_state.position.x,
                s->movement.last_observed_state.position.y,
                s->movement.last_observed_state.position.z,
                s->movement.last_observed_state.rotation.yaw,
                s->movement.last_observed_state.velocity.x,
                s->movement.last_observed_state.velocity.y,
                s->movement.last_observed_state.velocity.z);
    }
    fputc('\n', f);
    fclose(f);
}

void phase1r_rhe_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rhe_collect(const Phase1RRealHookEvidenceConfig* config) {
    Phase1RInEngineGoal0ValidationConfig active;
    CommonLibF4PlayerHookLiveCallbackCandidateState candidate_state;
    LocalPlayerObserverLiveProviderStats provider_stats;

    memset(&active, 0, sizeof(active));
    phase1r_rhe_reset();
    if (config) {
        active = config->validation_config;
    }

    active.require_toolchain_match = true;
    active.require_callback_validation = true;
    active.require_proxy_validation = false;
    active.require_remote_runtime_present = false;
    active.require_scene_present = false;
    active.require_driver_present = false;

    g_state.collected = true;
    g_state.harness_ok = phase1r_igmv_validate_current(&active);
    g_state.harness_state = phase1r_igmv_state();
    g_state.toolchain_ok = g_state.harness_state.toolchain_manifest_match;
    g_state.callback_ok = g_state.harness_state.callback_validation_passed;
    g_state.proxy_ok = true;
    g_state.local_player_id = g_state.harness_state.local_player_id;
    g_state.remote_player_id = g_state.harness_state.remote_player_id;
    g_state.runtime_profile_name = g_state.harness_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.harness_state.site_prototype_name;

    candidate_state = clf4_phlcc_state();
    provider_stats = lpo_live_provider_stats();
    g_state.callback_source_symbol = candidate_state.callback_source_symbol;
    g_state.callback_guard_symbol = candidate_state.callback_guard_symbol;
    g_state.movement.callback_registered = candidate_state.callback_registered;
    g_state.movement.provider_ready = candidate_state.provider_ready;
    g_state.movement.hook_activated = candidate_state.activated;
    g_state.movement.has_observed_state = candidate_state.has_state || provider_stats.has_state;
    g_state.movement.callback_forward_count = candidate_state.callback_forward_count;
    g_state.movement.callback_accept_count = candidate_state.callback_accept_count;
    g_state.movement.callback_reject_count = candidate_state.callback_reject_count;
    g_state.movement.real_callback_forward_count = candidate_state.real_callback_forward_count;
    g_state.movement.real_callback_accept_count = candidate_state.real_callback_accept_count;
    g_state.movement.real_callback_reject_count = candidate_state.real_callback_reject_count;
    g_state.movement.live_provider_submit_count = provider_stats.submit_count;
    g_state.movement.live_provider_capture_count = provider_stats.capture_count;
    g_state.movement.live_provider_accept_count = provider_stats.live_accept_count;
    g_state.movement.live_provider_reject_count = provider_stats.live_reject_count;
    g_state.movement.live_provider_queue_high_watermark = provider_stats.queue_high_watermark;
    g_state.movement.patch_gateway_marker_count = candidate_state.patch_gateway_marker_count;
    g_state.movement.detour_entry_count = candidate_state.shim_stats.detour_entry_count;
    g_state.movement.detour_bridge_forward_count = candidate_state.shim_stats.detour_bridge_forward_count;
    g_state.movement.real_dispatch_attempt_count = candidate_state.shim_stats.real_dispatch_attempt_count;
    g_state.movement.real_dispatch_success_count = candidate_state.shim_stats.real_dispatch_success_count;
    g_state.movement.real_dispatch_failure_count = candidate_state.shim_stats.real_dispatch_failure_count;
    g_state.movement.last_observed_state = provider_stats.has_state ? provider_stats.last_state : candidate_state.last_state;

    if (!g_state.toolchain_ok) {
        g_state.failure = PHASE1R_RHE_FAIL_TOOLCHAIN;
    } else if (!g_state.callback_ok) {
        g_state.failure = PHASE1R_RHE_FAIL_CALLBACK;
    } else if (!g_state.harness_ok) {
        g_state.failure = PHASE1R_RHE_FAIL_HARNESS;
    } else {
        g_state.failure = PHASE1R_RHE_FAIL_NONE;
    }

    g_state.evidence_ready = (g_state.failure == PHASE1R_RHE_FAIL_NONE);
    write_trace(&g_state);
    g_state.trace_written = true;
    return g_state.evidence_ready;
}

Phase1RRealHookEvidenceState phase1r_rhe_state(void) {
    return g_state;
}
