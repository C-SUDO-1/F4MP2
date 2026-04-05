#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_player_hook_attempt_stub.h"
#include "commonlibf4_player_hook_detour_site_candidate.h"
#include "commonlibf4_player_hook_detour_site_prototype.h"
#include "commonlibf4_player_hook_runtime_profile.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "commonlibf4_player_live_adapter.h"
#include "commonlibf4_hook_install_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookRuntimeProbeConfig {
    PlayerId expected_local_player_id;
    bool require_update_address;
    bool require_input_address;
    bool require_callback_after_install;
} CommonLibF4PlayerHookRuntimeProbeConfig;

typedef struct CommonLibF4PlayerHookRuntimeProbeState {
    CommonLibF4PlayerHookRuntimeProbeConfig config;
    bool configured;
    bool install_attempted;
    bool install_ready;
    bool callback_ready;
    bool hook_correct;
    uintptr_t resolved_update_addr;
    uintptr_t resolved_input_addr;
    const char* site_prototype_name;
    const char* update_symbol_name;
    const char* input_symbol_name;
    const char* runtime_profile_name;
    uint32_t observed_runtime_version;
    uint32_t required_runtime_version;
    bool runtime_profile_supported;
    const char* site_candidate_name;
    bool site_candidate_valid;
    bool site_candidate_runtime_supported;
    bool site_candidate_update_resolved;
    bool site_candidate_input_resolved;
    const char* patch_policy_name;
    bool dry_run_resolve_only;
    bool armed_install;
    bool patch_step_attempted;
    HookInstallState install_state;
    HookInstallError install_error;
    uint32_t callback_count;
    uint32_t rejected_callback_count;
    uint32_t mismatched_player_count;
    PlayerId last_seen_player_id;
    uint32_t last_observed_tick;
    CommonLibF4PlayerLiveRejectReason last_reject_reason;
    CommonLibF4PlayerLiveSample last_sample;
} CommonLibF4PlayerHookRuntimeProbeState;

void clf4_phrp_reset(void);
void clf4_phrp_configure(const CommonLibF4PlayerHookRuntimeProbeConfig* config);
bool clf4_phrp_install_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

bool clf4_phrp_install_with_prototype(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

bool clf4_phrp_install_with_candidate(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);
bool clf4_phrp_submit_callback(const CommonLibF4PlayerHookArgs* args);
CommonLibF4PlayerHookRuntimeProbeState clf4_phrp_state(void);

#ifdef __cplusplus
}
#endif
