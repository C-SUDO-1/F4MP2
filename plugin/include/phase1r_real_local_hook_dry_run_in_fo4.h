#pragma once

#include <stdbool.h>

#include "commonlibf4_player_hook_runtime_probe.h"
#include "phase1r_toolchain_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Phase1RRealLocalHookDryRunInFo4Config {
    PlayerId expected_local_player_id;
    const char* observed_f4se_build_text;
    bool plugins_directory_present;
    bool f4se_log_present;
    bool require_plugins_directory;
    bool require_manifest_runtime_match;
    bool require_manifest_f4se_match;
} Phase1RRealLocalHookDryRunInFo4Config;

typedef struct Phase1RRealLocalHookDryRunInFo4State {
    bool attempted;
    bool runtime_matches_manifest;
    bool f4se_matches_manifest;
    bool plugins_directory_present;
    bool f4se_log_present;
    bool dry_run_resolve_only;
    bool runtime_profile_supported;
    bool update_address_resolved;
    bool input_address_resolved;
    bool dry_run_ready;
    bool ready_for_armed_attempt;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    const char* manifest_runtime_text;
    const char* manifest_f4se_build_text;
    const char* observed_f4se_build_text;
    CommonLibF4PlayerHookRuntimeProbeState probe_state;
} Phase1RRealLocalHookDryRunInFo4State;

void phase1r_rlhdf_reset(void);
bool phase1r_rlhdf_run(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookDryRunInFo4Config* config
);
Phase1RRealLocalHookDryRunInFo4State phase1r_rlhdf_state(void);

#ifdef __cplusplus
}
#endif
