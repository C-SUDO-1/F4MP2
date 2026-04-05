#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PlayerHookCandidateId {
    PLAYER_HOOK_CANDIDATE_INVALID = 0,
    PLAYER_HOOK_CANDIDATE_UPDATE_BEFORE_INPUT,
    PLAYER_HOOK_CANDIDATE_UPDATE_AFTER_INPUT,
    PLAYER_HOOK_CANDIDATE_PLAYER_CONTROLS_UPDATE,
    PLAYER_HOOK_CANDIDATE_ACTOR_UPDATE_PLAYER_FILTERED
} PlayerHookCandidateId;

typedef enum HookAddressSource {
    HOOK_ADDRESS_SOURCE_UNRESOLVED = 0,
    HOOK_ADDRESS_SOURCE_STUB,
    HOOK_ADDRESS_SOURCE_ADDRESS_LIBRARY,
    HOOK_ADDRESS_SOURCE_PATTERN_SCAN,
    HOOK_ADDRESS_SOURCE_MANUAL_OFFSET,
    HOOK_ADDRESS_SOURCE_TRUSTED_PROFILE
} HookAddressSource;

typedef enum HookDiscoveryRecommendedAction {
    HOOK_DISCOVERY_ACTION_NONE = 0,
    HOOK_DISCOVERY_ACTION_INSTALL_LIVE,
    HOOK_DISCOVERY_ACTION_DRY_RUN_ONLY,
    HOOK_DISCOVERY_ACTION_REJECT_STUB_PROVIDER,
    HOOK_DISCOVERY_ACTION_COLLECT_REAL_ENTRY_PROBE,
    HOOK_DISCOVERY_ACTION_TRY_NEXT_CANDIDATE,
    HOOK_DISCOVERY_ACTION_UNSUPPORTED_RUNTIME
} HookDiscoveryRecommendedAction;

typedef enum HookDiscoveryRejectionReason {
    HOOK_DISCOVERY_REJECT_NONE = 0,
    HOOK_DISCOVERY_REJECT_UNSUPPORTED_RUNTIME,
    HOOK_DISCOVERY_REJECT_UNSUPPORTED_BUILD,
    HOOK_DISCOVERY_REJECT_UNRESOLVED_REQUIRED_ADDRESS,
    HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE,
    HOOK_DISCOVERY_REJECT_NON_EXECUTABLE_TARGET,
    HOOK_DISCOVERY_REJECT_WEAK_CONFIDENCE,
    HOOK_DISCOVERY_REJECT_PATCH_POLICY
} HookDiscoveryRejectionReason;

typedef struct HookDiscoveryRuntimeProfile {
    uint32_t runtime_version;
    bool runtime_supported;
    bool build_signature_supported;
    const char* profile_name;
} HookDiscoveryRuntimeProfile;

typedef struct PlayerHookDiscoveryCandidate {
    PlayerHookCandidateId candidate_id;
    const char* candidate_name;
    const char* source_symbol;
    const char* guard_symbol;
    uint32_t preferred_order;
    bool require_update_address;
    bool require_input_address;
} PlayerHookDiscoveryCandidate;

typedef struct PlayerHookDiscoveryResolution {
    bool resolved;
    uintptr_t update_addr;
    uintptr_t input_addr;
    HookAddressSource address_source;
    bool executable_memory_valid;
    bool runtime_supported;
    bool build_signature_supported;
} PlayerHookDiscoveryResolution;

typedef struct PlayerHookDiscoveryAssessment {
    PlayerHookDiscoveryCandidate candidate;
    PlayerHookDiscoveryResolution resolution;
    bool armable;
    bool dry_run_only;
    bool selected;
    HookDiscoveryRejectionReason rejection_reason;
    uint32_t score;
} PlayerHookDiscoveryAssessment;

typedef struct HookDiscoveryReport {
    HookDiscoveryRuntimeProfile runtime_profile;
    PlayerHookDiscoveryAssessment assessments[4];
    uint32_t assessment_count;
    PlayerHookCandidateId selected_candidate_id;
    bool simulated_only;
    bool real_execution_evidence;
    HookDiscoveryRecommendedAction recommended_action;
} HookDiscoveryReport;

void hdr_reset(HookDiscoveryReport* report);
void hdr_append_trace(const HookDiscoveryReport* report);

#ifdef __cplusplus
}
#endif
