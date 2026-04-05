#include "commonlibf4_player_hook_runtime_probe.h"

#include <stdio.h>
#include <string.h>

static CommonLibF4PlayerHookRuntimeProbeState g_probe;

static const char* TRACE_PATH =
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_runtime_probe_trace.txt";

static const CommonLibF4PlayerHookPatchPolicy* effective_policy(const CommonLibF4PlayerHookPatchPolicy* policy) {
    return policy ? policy : clf4_phpp_default_armed();
}

static void append_trace(const char* event_name, const CommonLibF4PlayerHookArgs* args) {
    FILE* f = NULL;
    fopen_s(&f, TRACE_PATH, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s configured=%d install_attempted=%d install_ready=%d callback_ready=%d hook_correct=%d runtime_profile_supported=%d callback_count=%u rejected_callback_count=%u mismatched_player_count=%u observed_runtime=0x%08X required_runtime=0x%08X patch_step_attempted=%d install_state=%d install_error=%d last_seen_player_id=%u last_observed_tick=%u last_reject_reason=%d",
            event_name ? event_name : "unknown",
            g_probe.configured ? 1 : 0,
            g_probe.install_attempted ? 1 : 0,
            g_probe.install_ready ? 1 : 0,
            g_probe.callback_ready ? 1 : 0,
            g_probe.hook_correct ? 1 : 0,
            g_probe.runtime_profile_supported ? 1 : 0,
            (unsigned int)g_probe.callback_count,
            (unsigned int)g_probe.rejected_callback_count,
            (unsigned int)g_probe.mismatched_player_count,
            (unsigned int)g_probe.observed_runtime_version,
            (unsigned int)g_probe.required_runtime_version,
            g_probe.patch_step_attempted ? 1 : 0,
            (int)g_probe.install_state,
            (int)g_probe.install_error,
            (unsigned int)g_probe.last_seen_player_id,
            (unsigned int)g_probe.last_observed_tick,
            (int)g_probe.last_reject_reason);
    if (args) {
        fprintf(f,
                " player_id=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
                (unsigned int)args->event.player_id,
                (unsigned int)args->event.stance,
                args->event.position.x,
                args->event.position.y,
                args->event.position.z,
                args->event.rotation.yaw,
                args->event.velocity.x,
                args->event.velocity.y,
                args->event.velocity.z);
    }
    fputc('\n', f);
    fclose(f);
}

static void refresh_hook_correctness(void) {
    const bool addresses_ready =
        (!g_probe.config.require_update_address || g_probe.resolved_update_addr != 0) &&
        (!g_probe.config.require_input_address || g_probe.resolved_input_addr != 0);
    const bool callbacks_ready =
        !g_probe.config.require_callback_after_install || g_probe.callback_count > 0;
    const bool player_ready = (g_probe.mismatched_player_count == 0);

    g_probe.callback_ready = callbacks_ready;
    g_probe.hook_correct =
        g_probe.install_ready &&
        g_probe.runtime_profile_supported &&
        addresses_ready &&
        callbacks_ready &&
        player_ready;
}

void clf4_phrp_reset(void) {
    memset(&g_probe, 0, sizeof(g_probe));
    hook_install_registry_reset();
    clf4_pla_reset(0);
    append_trace("reset", NULL);
}

void clf4_phrp_configure(const CommonLibF4PlayerHookRuntimeProbeConfig* config) {
    CommonLibF4PlayerHookRuntimeProbeConfig effective;
    memset(&effective, 0, sizeof(effective));
    effective.require_update_address = true;
    effective.require_input_address = true;
    effective.require_callback_after_install = true;
    if (config) {
        effective = *config;
    }
    g_probe.config = effective;
    g_probe.configured = true;
    clf4_pla_reset(g_probe.config.expected_local_player_id);
    refresh_hook_correctness();
    append_trace("configure", NULL);
}

bool clf4_phrp_install_with_prototype(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;
    CommonLibF4PlayerHookDetourSitePrototypeResolution resolution;
    HookAttemptResult result;
    const CommonLibF4PlayerHookPatchPolicy* active_policy = effective_policy(policy);

    if (!g_probe.configured) {
        CommonLibF4PlayerHookRuntimeProbeConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.require_update_address = true;
        cfg.require_input_address = true;
        cfg.require_callback_after_install = true;
        clf4_phrp_configure(&cfg);
    }

    g_probe.install_attempted = true;
    g_probe.install_ready = false;
    g_probe.resolved_update_addr = 0;
    g_probe.resolved_input_addr = 0;
    g_probe.site_prototype_name = NULL;
    g_probe.update_symbol_name = NULL;
    g_probe.input_symbol_name = NULL;
    g_probe.runtime_profile_name = NULL;
    g_probe.observed_runtime_version = f4se ? f4se->runtime_version : 0;
    g_probe.required_runtime_version = 0;
    g_probe.runtime_profile_supported = false;
    g_probe.site_candidate_name = NULL;
    g_probe.site_candidate_valid = false;
    g_probe.site_candidate_runtime_supported = false;
    g_probe.site_candidate_update_resolved = false;
    g_probe.site_candidate_input_resolved = false;
    g_probe.patch_policy_name = active_policy->name;
    g_probe.dry_run_resolve_only = clf4_phpp_is_dry_run(active_policy);
    g_probe.armed_install = clf4_phpp_is_armed(active_policy);
    g_probe.patch_step_attempted = false;

    clf4_phrprof_resolve(prototype_site, f4se, &profile_resolution);
    g_probe.runtime_profile_name = profile_resolution.profile_name;
    g_probe.observed_runtime_version = profile_resolution.observed_runtime_version;
    g_probe.required_runtime_version = profile_resolution.required_runtime_version;
    g_probe.runtime_profile_supported = profile_resolution.build_signature_supported;

    if (profile_resolution.build_signature_supported) {
        clf4_phdsp_resolve(prototype_site, f4se, provider, &resolution);
        g_probe.site_prototype_name = resolution.prototype_site ? resolution.prototype_site->name : NULL;
        g_probe.update_symbol_name = resolution.update_symbol_name;
        g_probe.input_symbol_name = resolution.input_symbol_name;
        g_probe.site_candidate_name = resolution.candidate_resolution.descriptor ? resolution.candidate_resolution.descriptor->name : NULL;
        g_probe.site_candidate_valid = resolution.candidate_resolution.valid;
        g_probe.site_candidate_runtime_supported = resolution.candidate_resolution.runtime_supported;
        g_probe.site_candidate_update_resolved = resolution.candidate_resolution.update_resolved;
        g_probe.site_candidate_input_resolved = resolution.candidate_resolution.input_resolved;
        g_probe.resolved_update_addr = resolution.candidate_resolution.update_addr;
        g_probe.resolved_input_addr = resolution.candidate_resolution.input_addr;
    } else {
        const CommonLibF4PlayerHookDetourSitePrototype* effective = prototype_site ? prototype_site : clf4_phdsp_default();
        g_probe.site_prototype_name = effective ? effective->name : NULL;
        g_probe.update_symbol_name = effective ? effective->update_symbol_name : NULL;
        g_probe.input_symbol_name = effective ? effective->input_symbol_name : NULL;
        g_probe.site_candidate_name = clf4_phdsp_descriptor(effective)->name;
    }

    result = clf4_attempt_install_player_hook_with_prototype(f4se, provider, prototype_site, patch_allowed, active_policy);
    g_probe.install_state = result.state;
    g_probe.install_error = result.error;
    g_probe.install_ready = (result.state == HOOK_INSTALL_INSTALLED);
    g_probe.patch_step_attempted = result.address_detail.patch_step_attempted;
    hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, result.state, result.error);
    refresh_hook_correctness();
    append_trace("install_with_prototype", NULL);
    return g_probe.install_ready;
}

bool clf4_phrp_install_with_candidate(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;
    CommonLibF4PlayerHookDetourSiteCandidateResolution resolution;
    HookAttemptResult result;
    const CommonLibF4PlayerHookPatchPolicy* active_policy = effective_policy(policy);

    if (!g_probe.configured) {
        CommonLibF4PlayerHookRuntimeProbeConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.require_update_address = true;
        cfg.require_input_address = true;
        cfg.require_callback_after_install = true;
        clf4_phrp_configure(&cfg);
    }

    g_probe.install_attempted = true;
    g_probe.install_ready = false;
    g_probe.resolved_update_addr = 0;
    g_probe.resolved_input_addr = 0;
    g_probe.site_prototype_name = NULL;
    g_probe.update_symbol_name = NULL;
    g_probe.input_symbol_name = NULL;
    g_probe.runtime_profile_name = NULL;
    g_probe.observed_runtime_version = f4se ? f4se->runtime_version : 0;
    g_probe.required_runtime_version = 0;
    g_probe.runtime_profile_supported = false;
    g_probe.site_candidate_name = NULL;
    g_probe.site_candidate_valid = false;
    g_probe.site_candidate_runtime_supported = false;
    g_probe.site_candidate_update_resolved = false;
    g_probe.site_candidate_input_resolved = false;
    g_probe.patch_policy_name = active_policy->name;
    g_probe.dry_run_resolve_only = clf4_phpp_is_dry_run(active_policy);
    g_probe.armed_install = clf4_phpp_is_armed(active_policy);
    g_probe.patch_step_attempted = false;

    clf4_phrprof_resolve(clf4_phdsp_default(), f4se, &profile_resolution);
    g_probe.runtime_profile_name = profile_resolution.profile_name;
    g_probe.observed_runtime_version = profile_resolution.observed_runtime_version;
    g_probe.required_runtime_version = profile_resolution.required_runtime_version;
    g_probe.runtime_profile_supported = profile_resolution.build_signature_supported;

    if (profile_resolution.build_signature_supported) {
        clf4_phdsc_resolve(descriptor, f4se, provider, &resolution);
        g_probe.site_candidate_name = resolution.descriptor ? resolution.descriptor->name : NULL;
        g_probe.site_candidate_valid = resolution.valid;
        g_probe.site_candidate_runtime_supported = resolution.runtime_supported;
        g_probe.site_candidate_update_resolved = resolution.update_resolved;
        g_probe.site_candidate_input_resolved = resolution.input_resolved;
        g_probe.resolved_update_addr = resolution.update_addr;
        g_probe.resolved_input_addr = resolution.input_addr;
    } else {
        g_probe.site_candidate_name = descriptor ? descriptor->name : NULL;
    }

    result = clf4_attempt_install_player_hook_with_candidate_descriptor(f4se, provider, descriptor, patch_allowed, active_policy);
    g_probe.install_state = result.state;
    g_probe.install_error = result.error;
    g_probe.install_ready = (result.state == HOOK_INSTALL_INSTALLED);
    g_probe.patch_step_attempted = result.address_detail.patch_step_attempted;
    hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, result.state, result.error);
    refresh_hook_correctness();
    append_trace("install_with_candidate", NULL);
    return g_probe.install_ready;
}

bool clf4_phrp_install_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    return clf4_phrp_install_with_prototype(f4se, provider, clf4_phdsp_default(), patch_allowed, policy);
}

bool clf4_phrp_submit_callback(const CommonLibF4PlayerHookArgs* args) {
    CommonLibF4PlayerLiveSample sample;
    CommonLibF4PlayerLiveAdapterStats adapter_stats;

    if (!g_probe.install_ready) {
        append_trace("submit_callback_not_ready", args);
        return false;
    }
    if (!clf4_pla_normalize(args, &sample)) {
        adapter_stats = clf4_pla_stats();
        g_probe.rejected_callback_count++;
        g_probe.last_reject_reason = adapter_stats.last_reject_reason;
        refresh_hook_correctness();
        append_trace("submit_callback_rejected", args);
        return false;
    }

    g_probe.callback_count++;
    g_probe.last_sample = sample;
    g_probe.last_seen_player_id = sample.state.player_id;
    g_probe.last_observed_tick = sample.observed_tick;
    g_probe.last_reject_reason = CLF4_PLA_REJECT_NONE;
    if (g_probe.config.expected_local_player_id != 0 && sample.state.player_id != g_probe.config.expected_local_player_id) {
        g_probe.mismatched_player_count++;
        refresh_hook_correctness();
        append_trace("submit_callback_mismatched_player", args);
        return false;
    }
    refresh_hook_correctness();
    append_trace("submit_callback_accept", args);
    return true;
}

CommonLibF4PlayerHookRuntimeProbeState clf4_phrp_state(void) {
    return g_probe;
}
