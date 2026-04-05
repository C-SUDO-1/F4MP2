#include "phase1r_in_engine_goal0_validation.h"

#include <string.h>

#include "phase1r_toolchain_manifest.h"
#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "proxy_runtime.h"

static Phase1RInEngineGoal0ValidationState g_state;

static Phase1RInEngineGoal0ValidationConfig default_config(void) {
    Phase1RInEngineGoal0ValidationConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.require_toolchain_match = true;
    cfg.require_callback_validation = true;
    cfg.require_proxy_validation = true;
    cfg.require_remote_runtime_present = true;
    cfg.require_scene_present = true;
    cfg.require_driver_present = true;
    cfg.callback_validation_config.min_callback_forward_count = 1;
    cfg.callback_validation_config.min_callback_accept_count = 1;
    cfg.callback_validation_config.require_runtime_profile_supported = true;
    cfg.callback_validation_config.require_callback_registered = true;
    cfg.callback_validation_config.require_provider_ready = true;
    cfg.callback_validation_config.require_captured_state = true;
    cfg.callback_validation_config.require_expected_player_id_match = true;
    return cfg;
}

void phase1r_igmv_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_igmv_validate_current(const Phase1RInEngineGoal0ValidationConfig* config) {
    Phase1RInEngineGoal0ValidationConfig active = default_config();
    CommonLibF4PlayerHookLiveCallbackCandidateState candidate = clf4_phlcc_state();
    const ProxyPlayerRecord* runtime_remote;
    const Fo4SceneProxyPlayerState* scene_remote;
    const Fo4ProxyActorState* driver_remote;

    phase1r_igmv_reset();
    if (config) active = *config;
    if (active.callback_validation_config.expected_local_player_id == 0) {
        active.callback_validation_config.expected_local_player_id = active.local_player_id;
    }

    g_state.evaluated = true;
    g_state.local_player_id = active.local_player_id;
    g_state.remote_player_id = active.remote_player_id;
    g_state.runtime_profile_name = candidate.runtime_profile_name;
    g_state.site_prototype_name = candidate.site_prototype_name;
    {
        const Phase1RToolchainManifest* manifest = phase1r_toolchain_manifest_current();
        g_state.toolchain_manifest_match = (manifest != 0 && candidate.runtime_profile_supported &&
            candidate.runtime_profile_name != 0 && manifest->runtime_profile_name != 0 &&
            strcmp(candidate.runtime_profile_name, manifest->runtime_profile_name) == 0);
    }

    g_state.callback_validation_passed = clf4_phlcv_validate_current(&active.callback_validation_config);
    g_state.callback_state = clf4_phlcv_state();
    if (g_state.local_player_id == 0) g_state.local_player_id = g_state.callback_state.observed_player_id;

    g_state.proxy_validation_passed = fpalv_validate_present(active.remote_player_id, &g_state.proxy_state);
    runtime_remote = proxy_runtime_get_player(active.remote_player_id);
    scene_remote = fo4_scene_proxy_backend_stub_get(active.remote_player_id);
    driver_remote = fpad_get_state(active.remote_player_id);
    g_state.remote_runtime_present = runtime_remote != 0;
    g_state.scene_present = scene_remote != 0;
    g_state.driver_present = driver_remote != 0;

    if (active.require_toolchain_match && !g_state.toolchain_manifest_match) {
        g_state.failure = PHASE1R_IGMV_FAIL_TOOLCHAIN_MISMATCH;
    } else if (active.require_callback_validation && !g_state.callback_validation_passed) {
        g_state.failure = PHASE1R_IGMV_FAIL_CALLBACK_VALIDATION;
    } else if (active.require_proxy_validation && !g_state.proxy_validation_passed) {
        g_state.failure = PHASE1R_IGMV_FAIL_PROXY_VALIDATION;
    } else if (active.require_remote_runtime_present && !g_state.remote_runtime_present) {
        g_state.failure = PHASE1R_IGMV_FAIL_REMOTE_RUNTIME_MISSING;
    } else if (active.require_scene_present && !g_state.scene_present) {
        g_state.failure = PHASE1R_IGMV_FAIL_SCENE_MISSING;
    } else if (active.require_driver_present && !g_state.driver_present) {
        g_state.failure = PHASE1R_IGMV_FAIL_DRIVER_MISSING;
    } else {
        g_state.failure = PHASE1R_IGMV_FAIL_NONE;
    }

    g_state.validated = (g_state.failure == PHASE1R_IGMV_FAIL_NONE);
    return g_state.validated;
}

Phase1RInEngineGoal0ValidationState phase1r_igmv_state(void) {
    return g_state;
}
