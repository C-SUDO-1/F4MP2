#include "commonlibf4_player_hook_live_callback_validation.h"

#include <string.h>

#include "local_player_observer.h"

static CommonLibF4PlayerHookLiveCallbackValidationState g_state;

static CommonLibF4PlayerHookLiveCallbackValidationConfig default_config(void) {
    CommonLibF4PlayerHookLiveCallbackValidationConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.min_callback_forward_count = 1;
    cfg.min_callback_accept_count = 1;
    cfg.require_runtime_profile_supported = true;
    cfg.require_callback_registered = true;
    cfg.require_provider_ready = true;
    cfg.require_captured_state = true;
    cfg.require_expected_player_id_match = true;
    return cfg;
}

void clf4_phlcv_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool clf4_phlcv_validate_current(const CommonLibF4PlayerHookLiveCallbackValidationConfig* config) {
    CommonLibF4PlayerHookLiveCallbackValidationConfig active = default_config();
    CommonLibF4PlayerHookLiveCallbackCandidateState candidate = clf4_phlcc_state();
    LocalPlayerObservedState captured;
    bool captured_ok = false;

    clf4_phlcv_reset();
    if (config) active = *config;

    g_state.evaluated = true;
    g_state.candidate_activated = candidate.activated;
    g_state.install_ready = candidate.install_ready;
    g_state.runtime_profile_supported = candidate.runtime_profile_supported;
    g_state.callback_registered = candidate.callback_registered;
    g_state.provider_ready = candidate.provider_ready;
    captured_ok = lpo_capture(&captured);
    g_state.has_state = candidate.has_state || captured_ok;
    g_state.expected_local_player_id = active.expected_local_player_id ? active.expected_local_player_id : candidate.expected_local_player_id;
    g_state.observed_player_id = captured_ok ? captured.player_id : (candidate.has_state ? candidate.last_state.player_id : 0);
    g_state.callback_forward_count = candidate.callback_forward_count;
    g_state.callback_accept_count = candidate.callback_accept_count;
    g_state.callback_reject_count = candidate.callback_reject_count;
    g_state.last_observed_tick = 0;
    g_state.site_prototype_name = candidate.site_prototype_name;
    g_state.runtime_profile_name = candidate.runtime_profile_name;
    g_state.callback_source_symbol = candidate.callback_source_symbol;
    g_state.callback_guard_symbol = candidate.callback_guard_symbol;
    g_state.failure = CLF4_PHLV_FAIL_NONE;

    if (!candidate.activated || !candidate.install_ready) {
        g_state.failure = CLF4_PHLV_FAIL_NOT_ACTIVATED;
    } else if (active.require_runtime_profile_supported && !candidate.runtime_profile_supported) {
        g_state.failure = CLF4_PHLV_FAIL_RUNTIME_PROFILE;
    } else if (active.require_callback_registered && !candidate.callback_registered) {
        g_state.failure = CLF4_PHLV_FAIL_CALLBACK_NOT_REGISTERED;
    } else if (active.require_provider_ready && !candidate.provider_ready) {
        g_state.failure = CLF4_PHLV_FAIL_PROVIDER_NOT_READY;
    } else if (candidate.callback_forward_count < active.min_callback_forward_count ||
               candidate.callback_accept_count < active.min_callback_accept_count) {
        g_state.failure = CLF4_PHLV_FAIL_NO_ACCEPTED_CALLBACK;
    } else if (active.require_captured_state && !g_state.has_state) {
        g_state.failure = CLF4_PHLV_FAIL_NO_CAPTURED_STATE;
    } else if (active.require_expected_player_id_match && g_state.expected_local_player_id != 0 &&
               g_state.observed_player_id != g_state.expected_local_player_id) {
        g_state.failure = CLF4_PHLV_FAIL_PLAYER_MISMATCH;
    }

    g_state.validated = (g_state.failure == CLF4_PHLV_FAIL_NONE);
    return g_state.validated;
}

CommonLibF4PlayerHookLiveCallbackValidationState clf4_phlcv_state(void) {
    return g_state;
}
