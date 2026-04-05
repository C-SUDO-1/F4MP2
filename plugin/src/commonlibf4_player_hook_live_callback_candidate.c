#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "player_hook_discovery.h"

#include <stdio.h>
#include <string.h>

static CommonLibF4PlayerHookLiveCallbackCandidateState g_state;
static unsigned int g_state_poll_count;

static const char* hook_address_source_label(HookAddressSource source) {
    switch (source) {
        case HOOK_ADDRESS_SOURCE_STUB: return "stub";
        case HOOK_ADDRESS_SOURCE_ADDRESS_LIBRARY: return "address_library";
        case HOOK_ADDRESS_SOURCE_PATTERN_SCAN: return "pattern_scan";
        case HOOK_ADDRESS_SOURCE_MANUAL_OFFSET: return "manual_offset";
        case HOOK_ADDRESS_SOURCE_UNRESOLVED:
        default:
            return "unresolved";
    }
}

static void append_trace(const char* event_name, const CommonLibF4PlayerHookLiveCallbackCandidateState* s) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_live_candidate_trace.txt";
    const bool real_callback_evidence = s && (s->real_callback_forward_count > 0u ||
                                              s->real_callback_accept_count > 0u ||
                                              s->real_callback_reject_count > 0u);
    const bool synthetic_only_success = s &&
        s->callback_forward_count == 1u && s->callback_accept_count == 1u && s->callback_reject_count == 0u &&
        s->real_callback_forward_count == 0u && s->real_callback_accept_count == 0u && s->real_callback_reject_count == 0u;
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s configured=%d activated=%d install_ready=%d runtime_profile_supported=%d provider_ready=%d callback_registered=%d forward=%u accept=%u reject=%u real_forward=%u real_accept=%u real_reject=%u has_state=%d real_callback_evidence=%d synthetic_only_success=%d state_poll_count=%u expected_local_player_id=%u detour_entry_count=%u detour_bridge_forward_count=%u shim_real_dispatch_attempts=%u shim_real_dispatch_success=%u shim_real_dispatch_failure=%u shim_last_thread_id=%u shim_has_last_return_address=%d shim_last_return_address=0x%p discovery_selected_id=%u discovery_selected=%s discovery_source=%s discovery_update=0x%p discovery_input=0x%p effective_patch_symbol=%s effective_patch_target=0x%p effective_patch_uses_input=%d planned_detour_entry=%s@0x%p planned_bridge_entry=%s@0x%p detour_destination=%s@0x%p patch_write_realized=%d bytes_overwritten=%u trampoline_realized=%d trampoline_addr=0x%p patch_realization_error=%u patch_realization_error_label=%s patch_realization_platform_error=%u patch_gateway_marker_count=%u runtime_profile=%s site=%s source=%s guard=%s\n",
            event_name ? event_name : "unknown",
            s && s->configured ? 1 : 0,
            s && s->activated ? 1 : 0,
            s && s->install_ready ? 1 : 0,
            s && s->runtime_profile_supported ? 1 : 0,
            s && s->provider_ready ? 1 : 0,
            s && s->callback_registered ? 1 : 0,
            s ? (unsigned int)s->callback_forward_count : 0,
            s ? (unsigned int)s->callback_accept_count : 0,
            s ? (unsigned int)s->callback_reject_count : 0,
            s ? (unsigned int)s->real_callback_forward_count : 0,
            s ? (unsigned int)s->real_callback_accept_count : 0,
            s ? (unsigned int)s->real_callback_reject_count : 0,
            s && s->has_state ? 1 : 0,
            real_callback_evidence ? 1 : 0,
            synthetic_only_success ? 1 : 0,
            g_state_poll_count,
            s ? (unsigned int)s->expected_local_player_id : 0,
            s ? (unsigned int)s->shim_stats.detour_entry_count : 0,
            s ? (unsigned int)s->shim_stats.detour_bridge_forward_count : 0,
            s ? (unsigned int)s->shim_stats.real_dispatch_attempt_count : 0,
            s ? (unsigned int)s->shim_stats.real_dispatch_success_count : 0,
            s ? (unsigned int)s->shim_stats.real_dispatch_failure_count : 0,
            s ? (unsigned int)s->shim_stats.last_thread_id : 0,
            s && s->shim_stats.has_last_return_address ? 1 : 0,
            s ? (void*)s->shim_stats.last_return_address : NULL,
            s ? (unsigned int)s->discovery_selected_candidate_id : 0u,
            (s && s->discovery_selected_candidate_name) ? s->discovery_selected_candidate_name : "",
            s ? hook_address_source_label(s->discovery_selected_address_source) : "unresolved",
            s ? (void*)s->discovery_selected_update_addr : NULL,
            s ? (void*)s->discovery_selected_input_addr : NULL,
            (s && s->effective_patch_target_symbol) ? s->effective_patch_target_symbol : "",
            s ? (void*)s->effective_patch_target_addr : NULL,
            s && s->effective_patch_target_uses_input ? 1 : 0,
            (s && s->planned_detour_entry_label) ? s->planned_detour_entry_label : "",
            s ? (void*)s->planned_detour_entry_addr : NULL,
            (s && s->planned_bridge_entry_label) ? s->planned_bridge_entry_label : "",
            s ? (void*)s->planned_bridge_entry_addr : NULL,
            (s && s->detour_destination_label) ? s->detour_destination_label : "",
            s ? (void*)s->detour_destination_addr : NULL,
            s && s->patch_write_realized ? 1 : 0,
            s ? (unsigned int)s->bytes_overwritten : 0u,
            s && s->trampoline_realized ? 1 : 0,
            s ? (void*)s->trampoline_addr : NULL,
            s ? (unsigned int)s->patch_realization_error_code : 0u,
            (s && s->patch_realization_error_label) ? s->patch_realization_error_label : "",
            s ? (unsigned int)s->patch_realization_platform_error_code : 0u,
            s ? (unsigned int)s->patch_gateway_marker_count : 0u,
            (s && s->runtime_profile_name) ? s->runtime_profile_name : "",
            (s && s->site_prototype_name) ? s->site_prototype_name : "",
            (s && s->callback_source_symbol) ? s->callback_source_symbol : "",
            (s && s->callback_guard_symbol) ? s->callback_guard_symbol : "");
    fclose(f);
}

static CommonLibF4PlayerHookLiveCallbackCandidateConfig default_config(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.install_config.require_hook_correct = true;
    cfg.install_config.patch_allowed = true;
    cfg.install_config.require_update_address = false;
    cfg.install_config.require_input_address = true;
    cfg.install_config.require_callback_after_install = true;
    cfg.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.install_config.site_prototype = clf4_phdsp_default();
    cfg.install_config.site_candidate = clf4_phdsp_descriptor(cfg.install_config.site_prototype);
    cfg.callback_source_symbol = "PlayerControls::Update";
    cfg.callback_guard_symbol = "PlayerControlsMovementData";
    cfg.require_supported_runtime_profile = true;
    return cfg;
}

void clf4_phlcc_reset(void) {
    clf4_phic_uninstall();
    memset(&g_state, 0, sizeof(g_state));
    g_state_poll_count = 0;
    append_trace("reset", &g_state);
}

bool clf4_phlcc_activate_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookLiveCallbackCandidateConfig* config
) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig active;
    CommonLibF4PlayerHookInstallCandidateStats stats;

    clf4_phlcc_reset();
    active = default_config();
    if (config) active = *config;
    if (!active.install_config.patch_policy) active.install_config.patch_policy = clf4_phpp_default_armed();
    if (!active.install_config.site_prototype) active.install_config.site_prototype = clf4_phdsp_default();
    if (!active.install_config.site_candidate) active.install_config.site_candidate = clf4_phdsp_descriptor(active.install_config.site_prototype);
    if (!active.callback_source_symbol) active.callback_source_symbol = "PlayerControls::Update";
    if (!active.callback_guard_symbol) active.callback_guard_symbol = "PlayerControlsMovementData";

    g_state.configured = true;
    g_state.callback_source_symbol = active.callback_source_symbol;
    g_state.callback_guard_symbol = active.callback_guard_symbol;
    g_state.expected_local_player_id = active.install_config.expected_local_player_id;

    {
        HookDiscoveryReport discovery_report;
        phd_run_player_discovery(f4se, provider, &discovery_report);
        hdr_append_trace(&discovery_report);
    }

    g_state.install_ready = clf4_phic_install_with_provider(f4se, provider, &active.install_config);
    stats = clf4_phic_stats();
    g_state.runtime_profile_supported = stats.runtime_profile_supported;
    g_state.site_prototype_name = stats.site_prototype_name;
    g_state.runtime_profile_name = stats.runtime_profile_name;
    g_state.provider_ready = stats.provider_installed;
    g_state.callback_registered = stats.callback_registered;
    g_state.callback_forward_count = stats.callback_forward_count;
    g_state.callback_accept_count = stats.callback_accept_count;
    g_state.callback_reject_count = stats.callback_reject_count;
    g_state.real_callback_forward_count = stats.real_callback_forward_count;
    g_state.real_callback_accept_count = stats.real_callback_accept_count;
    g_state.real_callback_reject_count = stats.real_callback_reject_count;
    g_state.discovery_selected_candidate_id = stats.discovery_selected_candidate_id;
    g_state.discovery_selected_candidate_name = stats.discovery_selected_candidate_name;
    g_state.discovery_selected_address_source = stats.discovery_selected_address_source;
    g_state.discovery_selected_update_addr = stats.discovery_selected_update_addr;
    g_state.discovery_selected_input_addr = stats.discovery_selected_input_addr;
    g_state.effective_patch_target_addr = stats.effective_patch_target_addr;
    g_state.effective_patch_target_symbol = stats.effective_patch_target_symbol;
    g_state.effective_patch_target_uses_input = stats.effective_patch_target_uses_input;
    g_state.planned_detour_entry_addr = stats.planned_detour_entry_addr;
    g_state.planned_detour_entry_label = stats.planned_detour_entry_label;
    g_state.planned_bridge_entry_addr = stats.planned_bridge_entry_addr;
    g_state.planned_bridge_entry_label = stats.planned_bridge_entry_label;
    g_state.patch_write_realized = stats.patch_write_realized;
    g_state.trampoline_realized = stats.trampoline_realized;
    g_state.trampoline_addr = stats.trampoline_addr;
    g_state.bytes_overwritten = stats.bytes_overwritten;
    g_state.detour_destination_addr = stats.detour_destination_addr;
    g_state.detour_destination_label = stats.detour_destination_label;
    g_state.patch_realization_error_code = stats.patch_realization_error_code;
    g_state.patch_realization_error_label = stats.patch_realization_error_label;
    g_state.patch_realization_platform_error_code = stats.patch_realization_platform_error_code;
    g_state.patch_gateway_marker_count = stats.patch_gateway_marker_count;
    g_state.has_state = stats.has_state;
    g_state.last_state = stats.last_state;
    g_state.shim_stats = phs_stats();
    g_state.activated = g_state.install_ready && g_state.callback_registered && g_state.provider_ready;
    if (active.require_supported_runtime_profile && !g_state.runtime_profile_supported) {
        g_state.activated = false;
    }
    append_trace("activate", &g_state);
    return g_state.activated;
}

bool clf4_phlcc_activated(void) {
    return g_state.activated;
}

CommonLibF4PlayerHookLiveCallbackCandidateState clf4_phlcc_state(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateState out = g_state;
    CommonLibF4PlayerHookInstallCandidateStats stats = clf4_phic_stats();

    g_state_poll_count++;
    out.install_ready = stats.install_ready;
    out.runtime_profile_supported = stats.runtime_profile_supported;
    out.site_prototype_name = stats.site_prototype_name;
    out.runtime_profile_name = stats.runtime_profile_name;
    out.provider_ready = stats.provider_installed;
    out.callback_registered = stats.callback_registered;
    out.callback_forward_count = stats.callback_forward_count;
    out.callback_accept_count = stats.callback_accept_count;
    out.callback_reject_count = stats.callback_reject_count;
    out.real_callback_forward_count = stats.real_callback_forward_count;
    out.real_callback_accept_count = stats.real_callback_accept_count;
    out.real_callback_reject_count = stats.real_callback_reject_count;
    out.discovery_selected_candidate_id = stats.discovery_selected_candidate_id;
    out.discovery_selected_candidate_name = stats.discovery_selected_candidate_name;
    out.discovery_selected_address_source = stats.discovery_selected_address_source;
    out.discovery_selected_update_addr = stats.discovery_selected_update_addr;
    out.discovery_selected_input_addr = stats.discovery_selected_input_addr;
    out.effective_patch_target_addr = stats.effective_patch_target_addr;
    out.effective_patch_target_symbol = stats.effective_patch_target_symbol;
    out.effective_patch_target_uses_input = stats.effective_patch_target_uses_input;
    out.planned_detour_entry_addr = stats.planned_detour_entry_addr;
    out.planned_detour_entry_label = stats.planned_detour_entry_label;
    out.planned_bridge_entry_addr = stats.planned_bridge_entry_addr;
    out.planned_bridge_entry_label = stats.planned_bridge_entry_label;
    out.patch_write_realized = stats.patch_write_realized;
    out.trampoline_realized = stats.trampoline_realized;
    out.trampoline_addr = stats.trampoline_addr;
    out.bytes_overwritten = stats.bytes_overwritten;
    out.detour_destination_addr = stats.detour_destination_addr;
    out.detour_destination_label = stats.detour_destination_label;
    out.patch_realization_error_code = stats.patch_realization_error_code;
    out.patch_realization_error_label = stats.patch_realization_error_label;
    out.patch_realization_platform_error_code = stats.patch_realization_platform_error_code;
    out.patch_gateway_marker_count = stats.patch_gateway_marker_count;
    out.has_state = stats.has_state;
    out.last_state = stats.last_state;
    out.shim_stats = phs_stats();

    if (out.callback_forward_count != g_state.callback_forward_count ||
        out.callback_accept_count != g_state.callback_accept_count ||
        out.callback_reject_count != g_state.callback_reject_count ||
        out.real_callback_forward_count != g_state.real_callback_forward_count ||
        out.real_callback_accept_count != g_state.real_callback_accept_count ||
        out.real_callback_reject_count != g_state.real_callback_reject_count ||
        out.has_state != g_state.has_state) {
        append_trace("state_refresh", &out);
    } else {
        append_trace("state_poll", &out);
    }

    g_state = out;
    return out;
}
