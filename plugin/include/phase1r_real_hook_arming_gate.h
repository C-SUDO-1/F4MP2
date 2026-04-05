#pragma once

#include <stdbool.h>

#include "phase1r_real_hook_evidence.h"
#include "commonlibf4_player_hook_live_callback_candidate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RRealHookArmingFailure {
    PHASE1R_RHAG_FAIL_NONE = 0,
    PHASE1R_RHAG_FAIL_NO_EVIDENCE,
    PHASE1R_RHAG_FAIL_CALLBACK_NOT_ACTIVATED,
    PHASE1R_RHAG_FAIL_RUNTIME_PROFILE,
    PHASE1R_RHAG_FAIL_PATCH_POLICY_NOT_ARMED
} Phase1RRealHookArmingFailure;

typedef struct Phase1RRealHookArmingGateConfig {
    Phase1RRealHookEvidenceConfig evidence_config;
    bool require_candidate_activated;
    bool require_runtime_profile_supported;
    bool require_patch_policy_armed;
} Phase1RRealHookArmingGateConfig;

typedef struct Phase1RRealHookArmingGateState {
    bool evaluated;
    bool arming_allowed;
    bool evidence_ready;
    bool candidate_activated;
    bool runtime_profile_supported;
    bool patch_policy_armed;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    Phase1RRealHookArmingFailure failure;
    Phase1RRealHookEvidenceState evidence_state;
    CommonLibF4PlayerHookLiveCallbackCandidateState candidate_state;
} Phase1RRealHookArmingGateState;

void phase1r_rhag_reset(void);
bool phase1r_rhag_evaluate(const Phase1RRealHookArmingGateConfig* config);
Phase1RRealHookArmingGateState phase1r_rhag_state(void);

#ifdef __cplusplus
}
#endif
