#include "commonlibf4_player_hook_install_candidate.h"
#include "player_hook_discovery.h"

#include <stdio.h>
#include <string.h>

#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_patch_realization.h"
#include "player_hook_shim.h"

typedef struct CommonLibF4PlayerHookInstallCandidateState {
    CommonLibF4PlayerHookInstallCandidateConfig config;
    bool configured;
    bool install_attempted;
    bool install_ready;
    bool adapter_installed;
    bool path_ready;
    HookDiscoveryReport discovery_report;
    bool have_discovery_report;
    CommonLibF4PlayerHookDetourSitePrototypeResolution configured_resolution;
    bool have_configured_resolution;
} CommonLibF4PlayerHookInstallCandidateState;

static CommonLibF4PlayerHookInstallCandidateState g_candidate;

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

static const PlayerHookDiscoveryAssessment* selected_assessment(const HookDiscoveryReport* report) {
    uint32_t i;
    if (!report) {
        return NULL;
    }
    for (i = 0; i < report->assessment_count; ++i) {
        if (report->assessments[i].selected) {
            return &report->assessments[i];
        }
    }
    return NULL;
}

static uintptr_t effective_patch_target_addr_for_config(
    const CommonLibF4PlayerHookInstallCandidateConfig* cfg,
    const CommonLibF4PlayerHookDetourSitePrototypeResolution* resolution
) {
    const CommonLibF4PlayerHookDetourSitePrototype* site =
        cfg && cfg->site_prototype ? cfg->site_prototype : clf4_phdsp_default();
    if (site && site->require_update_address) {
        return resolution ? resolution->candidate_resolution.update_addr : 0u;
    }
    return resolution ? resolution->candidate_resolution.input_addr : 0u;
}

static const char* effective_patch_target_symbol_for_config(const CommonLibF4PlayerHookInstallCandidateConfig* cfg) {
    const CommonLibF4PlayerHookDetourSitePrototype* site =
        cfg && cfg->site_prototype ? cfg->site_prototype : clf4_phdsp_default();
    if (site && site->require_update_address) {
        return site->update_symbol_name;
    }
    return site ? site->input_symbol_name : "";
}

static bool effective_patch_target_uses_input_for_config(const CommonLibF4PlayerHookInstallCandidateConfig* cfg) {
    const CommonLibF4PlayerHookDetourSitePrototype* site =
        cfg && cfg->site_prototype ? cfg->site_prototype : clf4_phdsp_default();
    return !(site && site->require_update_address);
}

static uintptr_t planned_detour_entry_addr_for_symbol(const char* patch_symbol) {
    if (patch_symbol && strcmp(patch_symbol, "CreatePlayerControlHandlers") == 0) {
        return (uintptr_t)&phs_create_player_controls_manual_detour_thunk;
    }
    return (uintptr_t)&phs_note_manual_detour_entry;
}

static const char* planned_detour_entry_label_for_symbol(const char* patch_symbol) {
    if (patch_symbol && strcmp(patch_symbol, "CreatePlayerControlHandlers") == 0) {
        return "phs_create_player_controls_manual_detour_thunk";
    }
    return "phs_note_manual_detour_entry";
}


static void append_patch_audit_trace(const char* event_name, const CommonLibF4PlayerHookInstallCandidateStats* s) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_patch_target_audit_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s discovery_selected_id=%u discovery_selected_name=%s discovery_source=%s discovery_update=0x%p discovery_input=0x%p discovery_armable=%d configured_site=%s configured_candidate=%s configured_update_symbol=%s configured_input_symbol=%s configured_update=0x%p configured_input=0x%p effective_patch_symbol=%s effective_patch_target=0x%p effective_patch_uses_input=%d planned_detour_entry=%s@0x%p planned_bridge_entry=%s@0x%p detour_destination=%s@0x%p patch_step_attempted=%d patch_write_realized=%d bytes_overwritten=%u trampoline_realized=%d trampoline_addr=0x%p patch_realization_error=%u patch_realization_error_label=%s patch_realization_platform_error=%u patch_gateway_marker_count=%u install_state=%d install_error=%d\n",
            event_name ? event_name : "unknown",
            s ? (unsigned int)s->discovery_selected_candidate_id : 0u,
            (s && s->discovery_selected_candidate_name) ? s->discovery_selected_candidate_name : "",
            s ? hook_address_source_label(s->discovery_selected_address_source) : "unresolved",
            s ? (void*)s->discovery_selected_update_addr : NULL,
            s ? (void*)s->discovery_selected_input_addr : NULL,
            s && s->discovery_selected_armable ? 1 : 0,
            (s && s->site_prototype_name) ? s->site_prototype_name : "",
            (s && s->site_candidate_name) ? s->site_candidate_name : "",
            (s && s->update_symbol_name) ? s->update_symbol_name : "",
            (s && s->input_symbol_name) ? s->input_symbol_name : "",
            s ? (void*)s->resolved_update_addr : NULL,
            s ? (void*)s->resolved_input_addr : NULL,
            (s && s->effective_patch_target_symbol) ? s->effective_patch_target_symbol : "",
            s ? (void*)s->effective_patch_target_addr : NULL,
            s && s->effective_patch_target_uses_input ? 1 : 0,
            (s && s->planned_detour_entry_label) ? s->planned_detour_entry_label : "",
            s ? (void*)s->planned_detour_entry_addr : NULL,
            (s && s->planned_bridge_entry_label) ? s->planned_bridge_entry_label : "",
            s ? (void*)s->planned_bridge_entry_addr : NULL,
            (s && s->detour_destination_label) ? s->detour_destination_label : "",
            s ? (void*)s->detour_destination_addr : NULL,
            s && s->patch_step_attempted ? 1 : 0,
            s && s->patch_write_realized ? 1 : 0,
            s ? (unsigned int)s->bytes_overwritten : 0u,
            s && s->trampoline_realized ? 1 : 0,
            s ? (void*)s->trampoline_addr : NULL,
            s ? (unsigned int)s->patch_realization_error_code : 0u,
            (s && s->patch_realization_error_label) ? s->patch_realization_error_label : "",
            s ? (unsigned int)s->patch_realization_platform_error_code : 0u,
            s ? (unsigned int)s->patch_gateway_marker_count : 0u,
            s ? (int)s->install_state : 0,
            s ? (int)s->install_error : 0);
    fclose(f);
}

static void append_trace(const char* event_name, const CommonLibF4PlayerHookInstallCandidateStats* s) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_install_candidate_trace.txt";
    const bool real_callback_evidence = s && (s->real_callback_forward_count > 0u ||
                                              s->real_callback_accept_count > 0u ||
                                              s->real_callback_reject_count > 0u);
    const bool synthetic_only_success = s && s->install_ready &&
        s->callback_accept_count == 1u && s->callback_forward_count == 1u && s->callback_reject_count == 0u &&
        s->real_callback_forward_count == 0u && s->real_callback_accept_count == 0u && s->real_callback_reject_count == 0u;
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s configured=%d install_attempted=%d install_ready=%d adapter_installed=%d callback_registered=%d provider_installed=%d hook_correct=%d path_ready=%d runtime_profile_supported=%d dry_run=%d armed_install=%d patch_step_attempted=%d install_state=%d install_error=%d forward=%u accept=%u reject=%u real_forward=%u real_accept=%u real_reject=%u has_state=%d expected_local_player_id=%u observed_runtime=0x%08X required_runtime=0x%08X resolved_update=0x%p resolved_input=0x%p discovery_selected_id=%u discovery_selected=%s discovery_source=%s discovery_update=0x%p discovery_input=0x%p effective_patch_symbol=%s effective_patch_target=0x%p effective_patch_uses_input=%d planned_detour_entry=%s@0x%p planned_bridge_entry=%s@0x%p detour_destination=%s@0x%p patch_write_realized=%d bytes_overwritten=%u trampoline_realized=%d trampoline_addr=0x%p patch_realization_error=%u patch_realization_error_label=%s patch_realization_platform_error=%u patch_gateway_marker_count=%u real_callback_evidence=%d synthetic_only_success=%d runtime_profile=%s site=%s\n",
            event_name ? event_name : "unknown",
            s && s->configured ? 1 : 0,
            s && s->install_attempted ? 1 : 0,
            s && s->install_ready ? 1 : 0,
            s && s->adapter_installed ? 1 : 0,
            s && s->callback_registered ? 1 : 0,
            s && s->provider_installed ? 1 : 0,
            s && s->hook_correct ? 1 : 0,
            s && s->path_ready ? 1 : 0,
            s && s->runtime_profile_supported ? 1 : 0,
            s && s->dry_run_resolve_only ? 1 : 0,
            s && s->armed_install ? 1 : 0,
            s && s->patch_step_attempted ? 1 : 0,
            s ? (int)s->install_state : 0,
            s ? (int)s->install_error : 0,
            s ? (unsigned int)s->callback_forward_count : 0,
            s ? (unsigned int)s->callback_accept_count : 0,
            s ? (unsigned int)s->callback_reject_count : 0,
            s ? (unsigned int)s->real_callback_forward_count : 0,
            s ? (unsigned int)s->real_callback_accept_count : 0,
            s ? (unsigned int)s->real_callback_reject_count : 0,
            s && s->has_state ? 1 : 0,
            s ? (unsigned int)s->expected_local_player_id : 0,
            s ? (unsigned int)s->observed_runtime_version : 0,
            s ? (unsigned int)s->required_runtime_version : 0,
            s ? (void*)s->resolved_update_addr : NULL,
            s ? (void*)s->resolved_input_addr : NULL,
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
            real_callback_evidence ? 1 : 0,
            synthetic_only_success ? 1 : 0,
            (s && s->runtime_profile_name) ? s->runtime_profile_name : "",
            (s && s->site_prototype_name) ? s->site_prototype_name : "");
    fclose(f);
}

static const CommonLibF4PlayerHookDetourSitePrototype* selected_prototype(void) {
    if (g_candidate.config.site_prototype) {
        return g_candidate.config.site_prototype;
    }
    return clf4_phdsp_default();
}

static const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* selected_descriptor(void) {
    if (g_candidate.config.site_candidate) {
        return g_candidate.config.site_candidate;
    }
    return clf4_phdsp_descriptor(selected_prototype());
}

static CommonLibF4PlayerHookInstallCandidateConfig default_config(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = false;
    cfg.require_callback_after_install = true;
    cfg.patch_policy = clf4_phpp_default_armed();
    cfg.site_prototype = clf4_phdsp_default();
    cfg.site_candidate = clf4_phdsp_descriptor(cfg.site_prototype);
    return cfg;
}

static void snapshot_probe_config(const CommonLibF4PlayerHookInstallCandidateConfig* cfg) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = cfg->expected_local_player_id;
    probe_cfg.require_update_address = cfg->require_update_address;
    probe_cfg.require_input_address = cfg->require_input_address;
    probe_cfg.require_callback_after_install = cfg->require_callback_after_install;
    clf4_phrp_configure(&probe_cfg);
}

void clf4_phic_reset(void) {
    clf4_phsca_uninstall();
    clf4_phrp_reset();
    clf4_phpr_reset();
    clf4_phpr_reset();
    memset(&g_candidate, 0, sizeof(g_candidate));
}

void clf4_phic_uninstall(void) {
    CommonLibF4PlayerHookInstallCandidateStats stats = clf4_phic_stats();
    append_trace("uninstall", &stats);
    append_patch_audit_trace("uninstall", &stats);
    clf4_phsca_uninstall();
    memset(&g_candidate, 0, sizeof(g_candidate));
}

bool clf4_phic_install_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookInstallCandidateConfig* config
) {
    CommonLibF4PlayerHookSourceCandidateAdapterConfig adapter_cfg;
    CommonLibF4PlayerHookCallbackStubStats callback_stats;
    CommonLibF4PlayerHookInstallCandidateStats stats;

    clf4_phic_reset();
    g_candidate.config = default_config();
    if (config) {
        g_candidate.config = *config;
        if (!g_candidate.config.patch_policy) {
            g_candidate.config.patch_policy = clf4_phpp_default_armed();
        }
        if (!g_candidate.config.site_prototype) {
            g_candidate.config.site_prototype = clf4_phdsp_default();
        }
        if (!g_candidate.config.site_candidate) {
            g_candidate.config.site_candidate = clf4_phdsp_descriptor(g_candidate.config.site_prototype);
        }
    }
    g_candidate.configured = true;
    stats = clf4_phic_stats();
    append_trace("install_begin", &stats);
    append_patch_audit_trace("install_begin", &stats);

    {
        HookDiscoveryReport discovery_report;
        hdr_reset(&discovery_report);
        phd_run_player_discovery(f4se, provider, &discovery_report);
        hdr_append_trace(&discovery_report);
        g_candidate.discovery_report = discovery_report;
        g_candidate.have_discovery_report = true;
        memset(&g_candidate.configured_resolution, 0, sizeof(g_candidate.configured_resolution));
        clf4_phdsp_resolve(selected_prototype(), f4se, provider, &g_candidate.configured_resolution);
        g_candidate.have_configured_resolution = true;
        stats = clf4_phic_stats();
        append_trace("install_discovery_snapshot", &stats);
        append_patch_audit_trace("install_discovery_snapshot", &stats);
        if (discovery_report.recommended_action == HOOK_DISCOVERY_ACTION_REJECT_STUB_PROVIDER ||
            discovery_report.recommended_action == HOOK_DISCOVERY_ACTION_UNSUPPORTED_RUNTIME ||
            discovery_report.selected_candidate_id == PLAYER_HOOK_CANDIDATE_INVALID) {
            g_candidate.install_attempted = false;
            g_candidate.install_ready = false;
            g_candidate.path_ready = false;
            stats = clf4_phic_stats();
            append_trace("install_discovery_rejected", &stats);
            append_patch_audit_trace("install_discovery_rejected", &stats);
            return false;
        }
    }

    snapshot_probe_config(&g_candidate.config);
    g_candidate.install_attempted = true;
    g_candidate.install_ready = clf4_phrp_install_with_prototype(
        f4se,
        provider,
        selected_prototype(),
        g_candidate.config.patch_allowed,
        g_candidate.config.patch_policy
    );
    if (!g_candidate.install_ready) {
        g_candidate.path_ready = false;
        stats = clf4_phic_stats();
        append_trace("install_probe_failed", &stats);
        append_patch_audit_trace("install_probe_failed", &stats);
        return false;
    }

    memset(&adapter_cfg, 0, sizeof(adapter_cfg));
    adapter_cfg.expected_local_player_id = g_candidate.config.expected_local_player_id;
    adapter_cfg.require_hook_correct = g_candidate.config.require_hook_correct;
    clf4_phsca_install(&adapter_cfg);
    g_candidate.adapter_installed = clf4_phsca_installed();

    callback_stats = clf4_player_hook_callback_stub_stats();
    g_candidate.path_ready = g_candidate.install_ready && g_candidate.adapter_installed && callback_stats.registered_callback;
    stats = clf4_phic_stats();
    append_trace("install_complete", &stats);
    append_patch_audit_trace("install_complete", &stats);
    return g_candidate.path_ready;
}

bool clf4_phic_installed(void) {
    return g_candidate.path_ready;
}

CommonLibF4PlayerHookInstallCandidateStats clf4_phic_stats(void) {
    CommonLibF4PlayerHookInstallCandidateStats out;
    CommonLibF4PlayerHookRuntimeProbeState probe;
    CommonLibF4PlayerHookSourceCandidateAdapterStats adapter;
    CommonLibF4PlayerHookCallbackStubStats callback;

    memset(&out, 0, sizeof(out));
    probe = clf4_phrp_state();
    adapter = clf4_phsca_stats();
    callback = clf4_player_hook_callback_stub_stats();

    out.configured = g_candidate.configured;
    out.install_attempted = g_candidate.install_attempted || probe.install_attempted;
    out.install_ready = g_candidate.install_ready && probe.install_ready;
    out.adapter_installed = g_candidate.adapter_installed || adapter.installed;
    out.callback_registered = callback.registered_callback;
    out.provider_installed = adapter.provider_installed;
    out.hook_correct = probe.hook_correct;
    out.path_ready = out.install_ready && out.adapter_installed && out.callback_registered && out.provider_installed;
    out.patch_policy_name = probe.patch_policy_name;
    out.dry_run_resolve_only = probe.dry_run_resolve_only;
    out.armed_install = probe.armed_install;
    out.patch_step_attempted = probe.patch_step_attempted;
    out.site_prototype_name = probe.site_prototype_name;
    out.update_symbol_name = probe.update_symbol_name;
    out.input_symbol_name = probe.input_symbol_name;
    out.runtime_profile_name = probe.runtime_profile_name;
    out.observed_runtime_version = probe.observed_runtime_version;
    out.required_runtime_version = probe.required_runtime_version;
    out.runtime_profile_supported = probe.runtime_profile_supported;
    out.site_candidate_name = probe.site_candidate_name;
    out.site_candidate_valid = probe.site_candidate_valid;
    out.site_candidate_runtime_supported = probe.site_candidate_runtime_supported;
    out.site_candidate_update_resolved = probe.site_candidate_update_resolved;
    out.site_candidate_input_resolved = probe.site_candidate_input_resolved;
    out.resolved_update_addr = probe.resolved_update_addr;
    out.resolved_input_addr = probe.resolved_input_addr;
    {
        const PlayerHookDiscoveryAssessment* selected = selected_assessment(g_candidate.have_discovery_report ? &g_candidate.discovery_report : NULL);
        out.discovery_selected_candidate_id = selected ? selected->candidate.candidate_id : PLAYER_HOOK_CANDIDATE_INVALID;
        out.discovery_selected_candidate_name = selected ? selected->candidate.candidate_name : NULL;
        out.discovery_selected_address_source = selected ? selected->resolution.address_source : HOOK_ADDRESS_SOURCE_UNRESOLVED;
        out.discovery_selected_update_addr = selected ? selected->resolution.update_addr : 0u;
        out.discovery_selected_input_addr = selected ? selected->resolution.input_addr : 0u;
        out.discovery_selected_armable = selected ? selected->armable : false;
        out.effective_patch_target_addr = effective_patch_target_addr_for_config(&g_candidate.config, g_candidate.have_configured_resolution ? &g_candidate.configured_resolution : NULL);
        out.effective_patch_target_symbol = effective_patch_target_symbol_for_config(&g_candidate.config);
        out.effective_patch_target_uses_input = effective_patch_target_uses_input_for_config(&g_candidate.config);
        out.planned_detour_entry_addr = planned_detour_entry_addr_for_symbol(out.effective_patch_target_symbol);
        out.planned_detour_entry_label = planned_detour_entry_label_for_symbol(out.effective_patch_target_symbol);
        CommonLibF4PlayerHookPatchRealizationState patch_state = clf4_phpr_state();
        out.planned_bridge_entry_addr = (uintptr_t)&phs_ingest_real_player_snapshot;
        out.planned_bridge_entry_label = "phs_ingest_real_player_snapshot";
        out.patch_write_realized = patch_state.patch_write_realized;
        out.trampoline_realized = patch_state.trampoline_realized;
        out.trampoline_addr = patch_state.trampoline_addr;
        out.bytes_overwritten = patch_state.bytes_overwritten;
        out.detour_destination_addr = patch_state.detour_destination_addr;
        out.detour_destination_label = patch_state.detour_destination_label;
        out.patch_realization_error_code = (uint32_t)patch_state.error;
        out.patch_realization_error_label = patch_state.error_label;
        out.patch_realization_platform_error_code = patch_state.platform_error_code;
        out.patch_gateway_marker_count = patch_state.gateway_marker_count;
    }
    out.install_state = probe.install_state;
    out.install_error = probe.install_error;
    out.callback_forward_count = adapter.callback_forward_count;
    out.callback_accept_count = adapter.callback_accept_count;
    out.callback_reject_count = adapter.callback_reject_count;
    out.real_callback_forward_count = adapter.real_callback_forward_count;
    out.real_callback_accept_count = adapter.real_callback_accept_count;
    out.real_callback_reject_count = adapter.real_callback_reject_count;
    out.last_probe_callback_count = adapter.last_probe_callback_count;
    out.expected_local_player_id = adapter.expected_local_player_id;
    out.has_state = adapter.has_state;
    out.last_state = adapter.last_state;
    return out;
}
