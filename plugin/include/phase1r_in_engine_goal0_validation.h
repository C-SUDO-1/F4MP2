#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "messages_player.h"
#include "commonlibf4_player_hook_live_callback_validation.h"
#include "fo4_proxy_actor_live_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RInEngineGoal0ValidationFailure {
    PHASE1R_IGMV_FAIL_NONE = 0,
    PHASE1R_IGMV_FAIL_TOOLCHAIN_MISMATCH,
    PHASE1R_IGMV_FAIL_CALLBACK_VALIDATION,
    PHASE1R_IGMV_FAIL_PROXY_VALIDATION,
    PHASE1R_IGMV_FAIL_REMOTE_RUNTIME_MISSING,
    PHASE1R_IGMV_FAIL_SCENE_MISSING,
    PHASE1R_IGMV_FAIL_DRIVER_MISSING
} Phase1RInEngineGoal0ValidationFailure;

typedef struct Phase1RInEngineGoal0ValidationConfig {
    PlayerId local_player_id;
    PlayerId remote_player_id;
    bool require_toolchain_match;
    bool require_callback_validation;
    bool require_proxy_validation;
    bool require_remote_runtime_present;
    bool require_scene_present;
    bool require_driver_present;
    CommonLibF4PlayerHookLiveCallbackValidationConfig callback_validation_config;
} Phase1RInEngineGoal0ValidationConfig;

typedef struct Phase1RInEngineGoal0ValidationState {
    bool evaluated;
    bool validated;
    bool toolchain_manifest_match;
    bool callback_validation_passed;
    bool proxy_validation_passed;
    bool remote_runtime_present;
    bool scene_present;
    bool driver_present;
    PlayerId local_player_id;
    PlayerId remote_player_id;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    CommonLibF4PlayerHookLiveCallbackValidationState callback_state;
    Fo4ProxyActorLiveValidationResult proxy_state;
    Phase1RInEngineGoal0ValidationFailure failure;
} Phase1RInEngineGoal0ValidationState;

void phase1r_igmv_reset(void);
bool phase1r_igmv_validate_current(const Phase1RInEngineGoal0ValidationConfig* config);
Phase1RInEngineGoal0ValidationState phase1r_igmv_state(void);

#ifdef __cplusplus
}
#endif
