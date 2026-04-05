#include "commonlibf4_player_hook_observer_bridge.h"
#include "local_player_observer_provider.h"
#include <string.h>

typedef struct CommonLibF4PlayerHookObserverBridgeState {
    bool installed;
    PlayerId local_player_id;
    uint32_t submit_count;
    uint32_t capture_count;
    uint32_t queue_high_watermark;
    uint32_t dropped_sample_count;
    uint32_t queue_count;
    uint32_t live_accept_count;
    uint32_t live_reject_count;
    uint32_t last_observed_tick;
    uint8_t last_source_kind;
    bool last_used_default_player_id;
    LocalPlayerObservedState last_state;
    bool has_state;
} CommonLibF4PlayerHookObserverBridgeState;

static CommonLibF4PlayerHookObserverBridgeState g_bridge;

static void sync_from_provider(void) {
    LocalPlayerObserverLiveProviderStats ps = lpo_live_provider_stats();
    g_bridge.submit_count = ps.submit_count;
    g_bridge.capture_count = ps.capture_count;
    g_bridge.queue_count = ps.queued_sample_count;
    g_bridge.queue_high_watermark = ps.queue_high_watermark;
    g_bridge.dropped_sample_count = ps.dropped_sample_count;
    g_bridge.live_accept_count = ps.live_accept_count;
    g_bridge.live_reject_count = ps.live_reject_count;
    g_bridge.last_observed_tick = ps.last_observed_tick;
    g_bridge.last_source_kind = ps.last_source_kind;
    g_bridge.last_used_default_player_id = ps.last_used_default_player_id;
    g_bridge.last_state = ps.last_state;
    g_bridge.has_state = ps.has_state;
}

void clf4_phob_install(PlayerId local_player_id) {
    memset(&g_bridge, 0, sizeof(g_bridge));
    g_bridge.installed = true;
    g_bridge.local_player_id = local_player_id;
    lpo_live_provider_install(local_player_id);
    sync_from_provider();
}

void clf4_phob_uninstall(void) {
    lpo_live_provider_uninstall();
    memset(&g_bridge, 0, sizeof(g_bridge));
}

bool clf4_phob_submit(const CommonLibF4PlayerHookArgs* args) {
    if (!args || !g_bridge.installed) return false;
    if (!lpo_live_provider_submit(args)) {
        sync_from_provider();
        return false;
    }
    sync_from_provider();
    return true;
}

CommonLibF4PlayerHookObserverBridgeStats clf4_phob_stats(void) {
    CommonLibF4PlayerHookObserverBridgeStats out;
    sync_from_provider();
    memset(&out, 0, sizeof(out));
    out.submit_count = g_bridge.submit_count;
    out.capture_count = g_bridge.capture_count;
    out.queued_sample_count = g_bridge.queue_count;
    out.queue_high_watermark = g_bridge.queue_high_watermark;
    out.dropped_sample_count = g_bridge.dropped_sample_count;
    out.live_accept_count = g_bridge.live_accept_count;
    out.live_reject_count = g_bridge.live_reject_count;
    out.last_observed_tick = g_bridge.last_observed_tick;
    out.last_source_kind = g_bridge.last_source_kind;
    out.last_used_default_player_id = g_bridge.last_used_default_player_id;
    out.provider_installed = g_bridge.installed;
    out.has_state = g_bridge.has_state;
    out.last_state = g_bridge.last_state;
    return out;
}
