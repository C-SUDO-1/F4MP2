#pragma once

#include <stdbool.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "commonlibf4_player_hook_live_callback_validation.h"
#include "movement_sync_bridge.h"
#include "phase1r_real_hook_wire_readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Phase1RRealLocalHookWiringConfig {
    Phase1RRealHookWireReadinessConfig readiness_config;
    CommonLibF4PlayerHookLiveCallbackCandidateConfig candidate_config;
    CommonLibF4PlayerHookLiveCallbackValidationConfig callback_validation_config;
    bool require_wire_ready_after_activation;
    bool emit_validation_sample;
    bool build_movement_message;
    CommonLibF4PlayerHookArgs validation_sample;
} Phase1RRealLocalHookWiringConfig;

typedef struct Phase1RRealLocalHookWiringState {
    bool attempted;
    bool activated;
    bool emitted_validation_sample;
    bool callback_validation_passed;
    bool wire_ready_after_activation;
    bool movement_message_built;
    bool wired;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    Phase1RRealHookWireAction recommended_action;
    CommonLibF4PlayerHookLiveCallbackCandidateState candidate_state;
    CommonLibF4PlayerHookLiveCallbackValidationState callback_validation_state;
    Phase1RRealHookWireReadinessState wire_readiness_state;
    MsgPlayerState emitted_message;
} Phase1RRealLocalHookWiringState;

void phase1r_rlhw_reset(void);
bool phase1r_rlhw_attempt(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookWiringConfig* config
);
Phase1RRealLocalHookWiringState phase1r_rlhw_state(void);

#ifdef __cplusplus
}
#endif
