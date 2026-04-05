#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_player_hook_install_candidate.h"
#include "player_hook_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookLiveCallbackCandidateConfig {
    CommonLibF4PlayerHookInstallCandidateConfig install_config;
    const char* callback_source_symbol;
    const char* callback_guard_symbol;
    bool require_supported_runtime_profile;
} CommonLibF4PlayerHookLiveCallbackCandidateConfig;

typedef struct CommonLibF4PlayerHookLiveCallbackCandidateState {
    bool configured;
    bool activated;
    bool install_ready;
    bool runtime_profile_supported;
    bool provider_ready;
    bool callback_registered;
    const char* site_prototype_name;
    const char* runtime_profile_name;
    const char* callback_source_symbol;
    const char* callback_guard_symbol;
    PlayerId expected_local_player_id;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t real_callback_forward_count;
    uint32_t real_callback_accept_count;
    uint32_t real_callback_reject_count;
    PlayerHookCandidateId discovery_selected_candidate_id;
    const char* discovery_selected_candidate_name;
    HookAddressSource discovery_selected_address_source;
    uintptr_t discovery_selected_update_addr;
    uintptr_t discovery_selected_input_addr;
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
    LocalPlayerObservedState last_state;
    bool has_state;
    PlayerHookShimStats shim_stats;
} CommonLibF4PlayerHookLiveCallbackCandidateState;

void clf4_phlcc_reset(void);
bool clf4_phlcc_activate_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookLiveCallbackCandidateConfig* config
);
bool clf4_phlcc_activated(void);
CommonLibF4PlayerHookLiveCallbackCandidateState clf4_phlcc_state(void);

#ifdef __cplusplus
}
#endif
