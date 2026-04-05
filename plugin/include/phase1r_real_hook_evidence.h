#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "local_player_observer_provider.h"
#include "phase1r_in_engine_goal0_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RRealHookEvidenceFailure {
    PHASE1R_RHE_FAIL_NONE = 0,
    PHASE1R_RHE_FAIL_TOOLCHAIN,
    PHASE1R_RHE_FAIL_CALLBACK,
    PHASE1R_RHE_FAIL_PROXY,
    PHASE1R_RHE_FAIL_HARNESS
} Phase1RRealHookEvidenceFailure;

typedef struct Phase1RRealHookEvidenceConfig {
    Phase1RInEngineGoal0ValidationConfig validation_config;
} Phase1RRealHookEvidenceConfig;

typedef struct Phase1RMovementSliceEvidence {
    bool callback_registered;
    bool provider_ready;
    bool hook_activated;
    bool has_observed_state;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t real_callback_forward_count;
    uint32_t real_callback_accept_count;
    uint32_t real_callback_reject_count;
    uint32_t live_provider_submit_count;
    uint32_t live_provider_capture_count;
    uint32_t live_provider_accept_count;
    uint32_t live_provider_reject_count;
    uint32_t live_provider_queue_high_watermark;
    uint32_t patch_gateway_marker_count;
    uint32_t detour_entry_count;
    uint32_t detour_bridge_forward_count;
    uint32_t real_dispatch_attempt_count;
    uint32_t real_dispatch_success_count;
    uint32_t real_dispatch_failure_count;
    LocalPlayerObservedState last_observed_state;
} Phase1RMovementSliceEvidence;

typedef struct Phase1RRealHookEvidenceState {
    bool collected;
    bool evidence_ready;
    bool toolchain_ok;
    bool callback_ok;
    bool proxy_ok;
    bool harness_ok;
    bool trace_written;
    PlayerId local_player_id;
    PlayerId remote_player_id;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    const char* callback_source_symbol;
    const char* callback_guard_symbol;
    Phase1RRealHookEvidenceFailure failure;
    Phase1RInEngineGoal0ValidationState harness_state;
    Phase1RMovementSliceEvidence movement;
} Phase1RRealHookEvidenceState;

void phase1r_rhe_reset(void);
bool phase1r_rhe_collect(const Phase1RRealHookEvidenceConfig* config);
Phase1RRealHookEvidenceState phase1r_rhe_state(void);

#ifdef __cplusplus
}
#endif
