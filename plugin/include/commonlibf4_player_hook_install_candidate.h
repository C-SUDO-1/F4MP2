#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_player_hook_source_candidate_adapter.h"
#include "commonlibf4_player_hook_runtime_probe.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "commonlibf4_player_hook_detour_site_candidate.h"
#include "commonlibf4_player_hook_detour_site_prototype.h"
#include "hook_discovery_report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookInstallCandidateConfig {
    PlayerId expected_local_player_id;
    bool require_hook_correct;
    bool patch_allowed;
    bool require_update_address;
    bool require_input_address;
    bool require_callback_after_install;
    const CommonLibF4PlayerHookPatchPolicy* patch_policy;
    const CommonLibF4PlayerHookDetourSitePrototype* site_prototype;
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* site_candidate;
} CommonLibF4PlayerHookInstallCandidateConfig;

typedef struct CommonLibF4PlayerHookInstallCandidateStats {
    bool configured;
    bool install_attempted;
    bool install_ready;
    bool adapter_installed;
    bool callback_registered;
    bool provider_installed;
    bool hook_correct;
    bool path_ready;
    const char* patch_policy_name;
    bool dry_run_resolve_only;
    bool armed_install;
    bool patch_step_attempted;
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
    uintptr_t resolved_update_addr;
    uintptr_t resolved_input_addr;
    PlayerHookCandidateId discovery_selected_candidate_id;
    const char* discovery_selected_candidate_name;
    HookAddressSource discovery_selected_address_source;
    uintptr_t discovery_selected_update_addr;
    uintptr_t discovery_selected_input_addr;
    bool discovery_selected_armable;
    uintptr_t effective_patch_target_addr;
    const char* effective_patch_target_symbol;
    bool effective_patch_target_uses_input;
    uintptr_t planned_detour_entry_addr;
    const char* planned_detour_entry_label;
    uintptr_t planned_bridge_entry_addr;
    const char* planned_bridge_entry_label;
    bool patch_write_realized;
    bool trampoline_realized;
    uintptr_t trampoline_addr;
    uint32_t bytes_overwritten;
    uintptr_t detour_destination_addr;
    const char* detour_destination_label;
    uint32_t patch_realization_error_code;
    const char* patch_realization_error_label;
    uint32_t patch_realization_platform_error_code;
    uint32_t patch_gateway_marker_count;
    HookInstallState install_state;
    HookInstallError install_error;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t real_callback_forward_count;
    uint32_t real_callback_accept_count;
    uint32_t real_callback_reject_count;
    uint32_t last_probe_callback_count;
    PlayerId expected_local_player_id;
    LocalPlayerObservedState last_state;
    bool has_state;
} CommonLibF4PlayerHookInstallCandidateStats;

void clf4_phic_reset(void);
void clf4_phic_uninstall(void);
bool clf4_phic_install_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookInstallCandidateConfig* config
);
bool clf4_phic_installed(void);
CommonLibF4PlayerHookInstallCandidateStats clf4_phic_stats(void);

#ifdef __cplusplus
}
#endif
