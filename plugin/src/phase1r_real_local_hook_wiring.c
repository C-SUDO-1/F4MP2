#include "phase1r_real_local_hook_wiring.h"

#include <stdio.h>
#include <string.h>

#include "player_hook_shim.h"

static Phase1RRealLocalHookWiringState g_state;

static void write_trace(
    const Phase1RRealLocalHookWiringState* s,
    unsigned int pre_forward,
    unsigned int pre_accept,
    unsigned int pre_reject,
    unsigned int real_forward,
    unsigned int real_accept,
    unsigned int real_reject
) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_wiring_runtime_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "w");
    if (!f || !s) {
        return;
    }

    fprintf(f, "attempted=%d\n", s->attempted ? 1 : 0);
    fprintf(f, "activated=%d\n", s->activated ? 1 : 0);
    fprintf(f, "emitted_validation_sample=%d\n", s->emitted_validation_sample ? 1 : 0);
    fprintf(f, "callback_validation_passed=%d\n", s->callback_validation_passed ? 1 : 0);
    fprintf(f, "wire_ready_after_activation=%d\n", s->wire_ready_after_activation ? 1 : 0);
    fprintf(f, "movement_message_built=%d\n", s->movement_message_built ? 1 : 0);
    fprintf(f, "wired=%d\n", s->wired ? 1 : 0);
    fprintf(f, "runtime_profile_name=%s\n", s->runtime_profile_name ? s->runtime_profile_name : "");
    fprintf(f, "site_prototype_name=%s\n", s->site_prototype_name ? s->site_prototype_name : "");
    fprintf(f, "candidate.callback_forward_count=%u\n", (unsigned int)s->candidate_state.callback_forward_count);
    fprintf(f, "candidate.callback_accept_count=%u\n", (unsigned int)s->candidate_state.callback_accept_count);
    fprintf(f, "candidate.callback_reject_count=%u\n", (unsigned int)s->candidate_state.callback_reject_count);
    fprintf(f, "candidate.real_callback_forward_count=%u\n", (unsigned int)s->candidate_state.real_callback_forward_count);
    fprintf(f, "candidate.real_callback_accept_count=%u\n", (unsigned int)s->candidate_state.real_callback_accept_count);
    fprintf(f, "candidate.real_callback_reject_count=%u\n", (unsigned int)s->candidate_state.real_callback_reject_count);
    fprintf(f, "candidate.has_state=%d\n", s->candidate_state.has_state ? 1 : 0);
    fprintf(f, "candidate.discovery_selected_id=%u\n", (unsigned int)s->candidate_state.discovery_selected_candidate_id);
    fprintf(f, "candidate.discovery_selected_name=%s\n", s->candidate_state.discovery_selected_candidate_name ? s->candidate_state.discovery_selected_candidate_name : "");
    fprintf(f, "candidate.discovery_source=%d\n", (int)s->candidate_state.discovery_selected_address_source);
    fprintf(f, "candidate.discovery_update=0x%p\n", (void*)s->candidate_state.discovery_selected_update_addr);
    fprintf(f, "candidate.discovery_input=0x%p\n", (void*)s->candidate_state.discovery_selected_input_addr);
    fprintf(f, "candidate.effective_patch_symbol=%s\n", s->candidate_state.effective_patch_target_symbol ? s->candidate_state.effective_patch_target_symbol : "");
    fprintf(f, "candidate.effective_patch_target=0x%p\n", (void*)s->candidate_state.effective_patch_target_addr);
    fprintf(f, "candidate.effective_patch_uses_input=%d\n", s->candidate_state.effective_patch_target_uses_input ? 1 : 0);
    fprintf(f, "candidate.planned_detour_entry=%s\n", s->candidate_state.planned_detour_entry_label ? s->candidate_state.planned_detour_entry_label : "");
    fprintf(f, "candidate.planned_detour_entry_addr=0x%p\n", (void*)s->candidate_state.planned_detour_entry_addr);
    fprintf(f, "candidate.planned_bridge_entry=%s\n", s->candidate_state.planned_bridge_entry_label ? s->candidate_state.planned_bridge_entry_label : "");
    fprintf(f, "candidate.planned_bridge_entry_addr=0x%p\n", (void*)s->candidate_state.planned_bridge_entry_addr);
    fprintf(f, "candidate.detour_destination=%s\n", s->candidate_state.detour_destination_label ? s->candidate_state.detour_destination_label : "");
    fprintf(f, "candidate.detour_destination_addr=0x%p\n", (void*)s->candidate_state.detour_destination_addr);
    fprintf(f, "candidate.patch_write_realized=%d\n", s->candidate_state.patch_write_realized ? 1 : 0);
    fprintf(f, "candidate.bytes_overwritten=%u\n", (unsigned int)s->candidate_state.bytes_overwritten);
    fprintf(f, "candidate.trampoline_realized=%d\n", s->candidate_state.trampoline_realized ? 1 : 0);
    fprintf(f, "candidate.trampoline_addr=0x%p\n", (void*)s->candidate_state.trampoline_addr);
    fprintf(f, "candidate.patch_realization_error=%u\n", s->candidate_state.patch_realization_error_code);
    fprintf(f, "candidate.patch_realization_error_label=%s\n", s->candidate_state.patch_realization_error_label ? s->candidate_state.patch_realization_error_label : "");
    fprintf(f, "candidate.patch_realization_platform_error=%u\n", s->candidate_state.patch_realization_platform_error_code);
    fprintf(f, "candidate.patch_gateway_marker_count=%u\n", s->candidate_state.patch_gateway_marker_count);
    fprintf(f, "pre_forward=%u\n", pre_forward);
    fprintf(f, "pre_accept=%u\n", pre_accept);
    fprintf(f, "pre_reject=%u\n", pre_reject);
    fprintf(f, "real_forward=%u\n", real_forward);
    fprintf(f, "real_accept=%u\n", real_accept);
    fprintf(f, "real_reject=%u\n", real_reject);
    fprintf(f, "real_callback_evidence=%d\n", (real_forward > 0u || real_accept > 0u || real_reject > 0u) ? 1 : 0);
    fprintf(f, "shim.detour_entry_count=%u\n", (unsigned int)s->candidate_state.shim_stats.detour_entry_count);
    fprintf(f, "shim.detour_bridge_forward_count=%u\n", (unsigned int)s->candidate_state.shim_stats.detour_bridge_forward_count);
    fprintf(f, "shim.real_dispatch_attempt_count=%u\n", (unsigned int)s->candidate_state.shim_stats.real_dispatch_attempt_count);
    fprintf(f, "shim.real_dispatch_success_count=%u\n", (unsigned int)s->candidate_state.shim_stats.real_dispatch_success_count);
    fprintf(f, "shim.real_dispatch_failure_count=%u\n", (unsigned int)s->candidate_state.shim_stats.real_dispatch_failure_count);
    fprintf(f, "shim.last_thread_id=%u\n", (unsigned int)s->candidate_state.shim_stats.last_thread_id);
    fprintf(f, "shim.has_last_return_address=%d\n", s->candidate_state.shim_stats.has_last_return_address ? 1 : 0);
    fprintf(f, "shim.last_return_address=0x%p\n", (void*)s->candidate_state.shim_stats.last_return_address);
    fprintf(f, "validation.validated=%d\n", s->callback_validation_state.validated ? 1 : 0);
    fprintf(f, "validation.failure=%d\n", (int)s->callback_validation_state.failure);
    fprintf(f, "readiness.ready_to_attempt_real_hook=%d\n", s->wire_readiness_state.ready_to_attempt_real_hook ? 1 : 0);
    fprintf(f, "readiness.recommended_action=%d\n", (int)s->recommended_action);
    fclose(f);
}

static Phase1RRealLocalHookWiringConfig default_config(void) {
    Phase1RRealLocalHookWiringConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.require_wire_ready_after_activation = true;
    cfg.emit_validation_sample = false;
    cfg.build_movement_message = false;
    cfg.callback_validation_config.min_callback_forward_count = 1;
    cfg.callback_validation_config.min_callback_accept_count = 1;
    cfg.callback_validation_config.require_runtime_profile_supported = true;
    cfg.callback_validation_config.require_callback_registered = true;
    cfg.callback_validation_config.require_provider_ready = true;
    cfg.callback_validation_config.require_captured_state = true;
    cfg.callback_validation_config.require_expected_player_id_match = true;
    cfg.candidate_config.install_config.require_hook_correct = true;
    cfg.candidate_config.install_config.patch_allowed = true;
    cfg.candidate_config.install_config.require_update_address = true;
    cfg.candidate_config.install_config.require_input_address = false;
    cfg.candidate_config.install_config.require_callback_after_install = true;
    cfg.candidate_config.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.candidate_config.install_config.site_prototype = clf4_phdsp_default();
    cfg.candidate_config.install_config.site_candidate = clf4_phdsp_descriptor(cfg.candidate_config.install_config.site_prototype);
    cfg.candidate_config.require_supported_runtime_profile = true;
    cfg.candidate_config.callback_source_symbol = "PlayerCharacter::Update";
    cfg.candidate_config.callback_guard_symbol = "PlayerControls::Update";
    return cfg;
}

void phase1r_rlhw_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rlhw_attempt(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookWiringConfig* config
) {
    Phase1RRealLocalHookWiringConfig active = default_config();
    CommonLibF4PlayerHookLiveCallbackCandidateState pre_candidate_state;
    unsigned int real_forward;
    unsigned int real_accept;
    unsigned int real_reject;

    phase1r_rlhw_reset();
    if (config) {
        active = *config;
        if (!active.candidate_config.install_config.patch_policy) {
            active.candidate_config.install_config.patch_policy = clf4_phpp_default_armed();
        }
        if (!active.candidate_config.install_config.site_prototype) {
            active.candidate_config.install_config.site_prototype = clf4_phdsp_default();
        }
        if (!active.candidate_config.install_config.site_candidate) {
            active.candidate_config.install_config.site_candidate = clf4_phdsp_descriptor(active.candidate_config.install_config.site_prototype);
        }
        if (!active.candidate_config.callback_source_symbol) {
            active.candidate_config.callback_source_symbol = "PlayerCharacter::Update";
        }
        if (!active.candidate_config.callback_guard_symbol) {
            active.candidate_config.callback_guard_symbol = "PlayerControls::Update";
        }
    }

    g_state.attempted = true;
    g_state.activated = clf4_phlcc_activate_with_provider(f4se, provider, &active.candidate_config);
    g_state.candidate_state = clf4_phlcc_state();
    pre_candidate_state = g_state.candidate_state;
    g_state.runtime_profile_name = g_state.candidate_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.candidate_state.site_prototype_name;

    if (g_state.activated && active.emit_validation_sample) {
        g_state.emitted_validation_sample = clf4_player_hook_callback_stub_emit(&active.validation_sample);
    }

    g_state.callback_validation_passed = clf4_phlcv_validate_current(&active.callback_validation_config);
    g_state.callback_validation_state = clf4_phlcv_state();

    phase1r_rhwr_evaluate(&active.readiness_config);
    g_state.wire_readiness_state = phase1r_rhwr_state();
    g_state.recommended_action = g_state.wire_readiness_state.recommended_action;

    if (g_state.activated && g_state.callback_validation_passed && active.build_movement_message) {
        memset(&g_state.emitted_message, 0, sizeof(g_state.emitted_message));
        g_state.movement_message_built = msb_build_player_state_if_changed(&g_state.emitted_message);
    }

    g_state.candidate_state = clf4_phlcc_state();
    g_state.runtime_profile_name = g_state.candidate_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.candidate_state.site_prototype_name;

    real_forward = g_state.candidate_state.real_callback_forward_count;
    real_accept = g_state.candidate_state.real_callback_accept_count;
    real_reject = g_state.candidate_state.real_callback_reject_count;

    g_state.wire_ready_after_activation =
        g_state.wire_readiness_state.ready_to_attempt_real_hook &&
        (real_forward > 0u || real_accept > 0u || real_reject > 0u);

    g_state.wired = g_state.activated &&
                    g_state.callback_validation_passed &&
                    (!active.emit_validation_sample || g_state.emitted_validation_sample) &&
                    (!active.build_movement_message || g_state.movement_message_built) &&
                    (!active.require_wire_ready_after_activation || g_state.wire_ready_after_activation);
    write_trace(&g_state,
                pre_candidate_state.callback_forward_count,
                pre_candidate_state.callback_accept_count,
                pre_candidate_state.callback_reject_count,
                real_forward,
                real_accept,
                real_reject);
    return g_state.wired;
}

Phase1RRealLocalHookWiringState phase1r_rlhw_state(void) {
    if (g_state.attempted) {
        CommonLibF4PlayerHookLiveCallbackCandidateState refreshed = clf4_phlcc_state();
        unsigned int real_forward;
        unsigned int real_accept;
        unsigned int real_reject;

        g_state.candidate_state = refreshed;
        g_state.runtime_profile_name = refreshed.runtime_profile_name;
        g_state.site_prototype_name = refreshed.site_prototype_name;

        real_forward = refreshed.real_callback_forward_count;
        real_accept = refreshed.real_callback_accept_count;
        real_reject = refreshed.real_callback_reject_count;
        g_state.wire_ready_after_activation =
            g_state.wire_readiness_state.ready_to_attempt_real_hook &&
            (real_forward > 0u || real_accept > 0u || real_reject > 0u || refreshed.patch_gateway_marker_count > 0u ||
             refreshed.shim_stats.detour_entry_count > 0u || refreshed.shim_stats.detour_bridge_forward_count > 0u);
    }
    return g_state;
}
