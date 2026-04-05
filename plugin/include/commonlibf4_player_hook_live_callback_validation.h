#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4PlayerHookLiveCallbackValidationFailure {
    CLF4_PHLV_FAIL_NONE = 0,
    CLF4_PHLV_FAIL_NOT_ACTIVATED,
    CLF4_PHLV_FAIL_RUNTIME_PROFILE,
    CLF4_PHLV_FAIL_CALLBACK_NOT_REGISTERED,
    CLF4_PHLV_FAIL_PROVIDER_NOT_READY,
    CLF4_PHLV_FAIL_NO_ACCEPTED_CALLBACK,
    CLF4_PHLV_FAIL_NO_CAPTURED_STATE,
    CLF4_PHLV_FAIL_PLAYER_MISMATCH
} CommonLibF4PlayerHookLiveCallbackValidationFailure;

typedef struct CommonLibF4PlayerHookLiveCallbackValidationConfig {
    PlayerId expected_local_player_id;
    uint32_t min_callback_forward_count;
    uint32_t min_callback_accept_count;
    bool require_runtime_profile_supported;
    bool require_callback_registered;
    bool require_provider_ready;
    bool require_captured_state;
    bool require_expected_player_id_match;
} CommonLibF4PlayerHookLiveCallbackValidationConfig;

typedef struct CommonLibF4PlayerHookLiveCallbackValidationState {
    bool evaluated;
    bool validated;
    bool candidate_activated;
    bool install_ready;
    bool runtime_profile_supported;
    bool callback_registered;
    bool provider_ready;
    bool has_state;
    PlayerId expected_local_player_id;
    PlayerId observed_player_id;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t last_observed_tick;
    const char* site_prototype_name;
    const char* runtime_profile_name;
    const char* callback_source_symbol;
    const char* callback_guard_symbol;
    CommonLibF4PlayerHookLiveCallbackValidationFailure failure;
} CommonLibF4PlayerHookLiveCallbackValidationState;

void clf4_phlcv_reset(void);
bool clf4_phlcv_validate_current(const CommonLibF4PlayerHookLiveCallbackValidationConfig* config);
CommonLibF4PlayerHookLiveCallbackValidationState clf4_phlcv_state(void);

#ifdef __cplusplus
}
#endif
