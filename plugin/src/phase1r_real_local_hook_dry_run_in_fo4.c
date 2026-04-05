#include "phase1r_real_local_hook_dry_run_in_fo4.h"

#include <string.h>

static Phase1RRealLocalHookDryRunInFo4State g_state;

void phase1r_rlhdf_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_rlhdf_run(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookDryRunInFo4Config* config
) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    const Phase1RToolchainManifest* manifest = phase1r_toolchain_manifest_current();
    Phase1RRealLocalHookDryRunInFo4Config active;

    memset(&active, 0, sizeof(active));
    active.require_plugins_directory = true;
    active.require_manifest_runtime_match = true;
    active.require_manifest_f4se_match = true;
    if (config) active = *config;

    phase1r_rlhdf_reset();
    g_state.attempted = true;
    g_state.manifest_runtime_text = manifest ? manifest->fallout_runtime_version_text : NULL;
    g_state.manifest_f4se_build_text = manifest ? manifest->f4se_build_text : NULL;
    g_state.observed_f4se_build_text = active.observed_f4se_build_text;
    g_state.plugins_directory_present = active.plugins_directory_present;
    g_state.f4se_log_present = active.f4se_log_present;

    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = active.expected_local_player_id;
    probe_cfg.require_update_address = true;
    probe_cfg.require_input_address = true;
    probe_cfg.require_callback_after_install = false;
    clf4_phrp_reset();
    clf4_phrp_configure(&probe_cfg);
    (void)clf4_phrp_install_with_prototype(f4se, provider, clf4_phdsp_default(), true, clf4_phpp_default_dry_run());

    g_state.probe_state = clf4_phrp_state();
    g_state.runtime_profile_name = g_state.probe_state.runtime_profile_name;
    g_state.site_prototype_name = g_state.probe_state.site_prototype_name;
    g_state.runtime_profile_supported = g_state.probe_state.runtime_profile_supported;
    g_state.update_address_resolved = g_state.probe_state.site_candidate_update_resolved;
    g_state.input_address_resolved = g_state.probe_state.site_candidate_input_resolved;
    g_state.dry_run_resolve_only = g_state.probe_state.dry_run_resolve_only;

    g_state.runtime_matches_manifest = manifest && f4se &&
        f4se->runtime_version == manifest->fallout_runtime_version_hex;
    g_state.f4se_matches_manifest = manifest && active.observed_f4se_build_text &&
        strcmp(active.observed_f4se_build_text, manifest->f4se_build_text) == 0;

    g_state.dry_run_ready =
        g_state.dry_run_resolve_only &&
        g_state.runtime_profile_supported &&
        g_state.update_address_resolved &&
        g_state.input_address_resolved &&
        (!active.require_plugins_directory || g_state.plugins_directory_present) &&
        (!active.require_manifest_runtime_match || g_state.runtime_matches_manifest) &&
        (!active.require_manifest_f4se_match || g_state.f4se_matches_manifest);

    g_state.ready_for_armed_attempt = g_state.dry_run_ready;
    return g_state.ready_for_armed_attempt;
}

Phase1RRealLocalHookDryRunInFo4State phase1r_rlhdf_state(void) {
    return g_state;
}
