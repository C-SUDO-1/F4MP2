#include "commonlibf4_local_player_probe_provider.h"

#include <string.h>

#include "local_player_observer_provider.h"

typedef struct CommonLibF4LocalPlayerProbeProviderState {
    CommonLibF4LocalPlayerProbeProviderConfig config;
    bool installed;
    bool has_state;
    uint32_t capture_count;
    uint32_t stale_capture_count;
    uint32_t blocked_capture_count;
    uint32_t last_callback_count;
    LocalPlayerObservedState last_state;
} CommonLibF4LocalPlayerProbeProviderState;

static CommonLibF4LocalPlayerProbeProviderState g_probe_provider;

static bool probe_provider_capture(void* user, LocalPlayerObservedState* out_state) {
    CommonLibF4LocalPlayerProbeProviderState* s = (CommonLibF4LocalPlayerProbeProviderState*)user;
    CommonLibF4PlayerHookRuntimeProbeState probe;

    if (!s || !out_state || !s->installed) return false;

    probe = clf4_phrp_state();
    if (!probe.install_ready || probe.callback_count == 0) {
        s->blocked_capture_count++;
        return false;
    }
    if (s->config.require_hook_correct && !probe.hook_correct) {
        s->blocked_capture_count++;
        return false;
    }
    if (s->config.expected_local_player_id != 0 &&
        probe.last_sample.state.player_id != s->config.expected_local_player_id) {
        s->blocked_capture_count++;
        return false;
    }

    *out_state = probe.last_sample.state;
    out_state->valid = true;

    if (probe.callback_count == s->last_callback_count && s->has_state) {
        s->stale_capture_count++;
    }
    s->last_callback_count = probe.callback_count;
    s->last_state = *out_state;
    s->has_state = true;
    s->capture_count++;
    return true;
}

void clf4_lppp_install(const CommonLibF4LocalPlayerProbeProviderConfig* config) {
    LocalPlayerObserverProvider provider;
    memset(&g_probe_provider, 0, sizeof(g_probe_provider));
    if (config) {
        g_probe_provider.config = *config;
    }
    g_probe_provider.installed = true;
    provider.user = &g_probe_provider;
    provider.capture = probe_provider_capture;
    lpo_install_provider(&provider);
}

void clf4_lppp_uninstall(void) {
    lpo_clear_provider();
    memset(&g_probe_provider, 0, sizeof(g_probe_provider));
}

bool clf4_lppp_installed(void) {
    return g_probe_provider.installed;
}

CommonLibF4LocalPlayerProbeProviderStats clf4_lppp_stats(void) {
    CommonLibF4LocalPlayerProbeProviderStats out;
    memset(&out, 0, sizeof(out));
    out.installed = g_probe_provider.installed;
    out.has_state = g_probe_provider.has_state;
    out.require_hook_correct = g_probe_provider.config.require_hook_correct;
    out.capture_count = g_probe_provider.capture_count;
    out.stale_capture_count = g_probe_provider.stale_capture_count;
    out.blocked_capture_count = g_probe_provider.blocked_capture_count;
    out.last_callback_count = g_probe_provider.last_callback_count;
    out.expected_local_player_id = g_probe_provider.config.expected_local_player_id;
    out.last_state = g_probe_provider.last_state;
    return out;
}
