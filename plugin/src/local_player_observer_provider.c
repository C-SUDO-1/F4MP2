#include "local_player_observer_provider.h"
#include "local_player_observer_stats.h"
#include <string.h>

#define LPO_LIVE_PROVIDER_MAX_QUEUE 8u

static LocalPlayerObserverProvider g_provider;

typedef struct LocalPlayerObserverLiveProviderState {
    bool installed;
    uint32_t submit_count;
    uint32_t capture_count;
    uint32_t queue_count;
    uint32_t queue_head;
    uint32_t queue_tail;
    uint32_t queue_high_watermark;
    uint32_t dropped_sample_count;
    uint32_t live_accept_count;
    uint32_t live_reject_count;
    uint32_t last_observed_tick;
    uint32_t last_accepted_sequence;
    uint8_t last_source_kind;
    bool last_used_default_player_id;
    CommonLibF4PlayerLiveRejectReason last_reject_reason;
    bool has_state;
    LocalPlayerObservedState last_state;
    LocalPlayerObservedState queue[LPO_LIVE_PROVIDER_MAX_QUEUE];
} LocalPlayerObserverLiveProviderState;

static LocalPlayerObserverLiveProviderState g_live_provider;

static bool provider_active(void) {
    return g_provider.capture != 0 || g_live_provider.installed;
}

static void sync_provider_installed_stat(void) {
    lpo_stats_note_provider_installed(provider_active());
}

static void live_queue_push(const LocalPlayerObservedState* state) {
    if (g_live_provider.queue_count == LPO_LIVE_PROVIDER_MAX_QUEUE) {
        g_live_provider.queue_head = (g_live_provider.queue_head + 1u) % LPO_LIVE_PROVIDER_MAX_QUEUE;
        g_live_provider.queue_count--;
        g_live_provider.dropped_sample_count++;
    }
    g_live_provider.queue[g_live_provider.queue_tail] = *state;
    g_live_provider.queue_tail = (g_live_provider.queue_tail + 1u) % LPO_LIVE_PROVIDER_MAX_QUEUE;
    g_live_provider.queue_count++;
    if (g_live_provider.queue_count > g_live_provider.queue_high_watermark) {
        g_live_provider.queue_high_watermark = g_live_provider.queue_count;
    }
}

static bool live_queue_peek_latest(LocalPlayerObservedState* out_state) {
    uint32_t idx;
    if (!out_state || g_live_provider.queue_count == 0) return false;
    idx = (g_live_provider.queue_tail + LPO_LIVE_PROVIDER_MAX_QUEUE - 1u) % LPO_LIVE_PROVIDER_MAX_QUEUE;
    *out_state = g_live_provider.queue[idx];
    return true;
}

static bool live_provider_capture(LocalPlayerObservedState* out_state) {
    if (!out_state || !g_live_provider.installed) return false;
    if (!live_queue_peek_latest(out_state)) {
        if (!g_live_provider.has_state) return false;
        *out_state = g_live_provider.last_state;
    }
    out_state->valid = true;
    g_live_provider.last_state = *out_state;
    g_live_provider.has_state = true;
    g_live_provider.capture_count++;
    return true;
}

void lpo_install_provider(const LocalPlayerObserverProvider* provider) {
    if (!provider) {
        memset(&g_provider, 0, sizeof(g_provider));
        sync_provider_installed_stat();
        return;
    }
    g_provider = *provider;
    sync_provider_installed_stat();
}

void lpo_clear_provider(void) {
    memset(&g_provider, 0, sizeof(g_provider));
    sync_provider_installed_stat();
}

bool lpo_has_provider(void) {
    return provider_active();
}

void lpo_live_provider_install(PlayerId fallback_player_id) {
    memset(&g_live_provider, 0, sizeof(g_live_provider));
    g_live_provider.installed = true;
    clf4_pla_reset(fallback_player_id);
    sync_provider_installed_stat();
}

void lpo_live_provider_uninstall(void) {
    memset(&g_live_provider, 0, sizeof(g_live_provider));
    clf4_pla_reset(0);
    sync_provider_installed_stat();
}

bool lpo_live_provider_installed(void) {
    return g_live_provider.installed;
}

bool lpo_live_provider_submit(const CommonLibF4PlayerHookArgs* args) {
    CommonLibF4PlayerLiveSample sample;
    CommonLibF4PlayerLiveAdapterStats adapter_stats;

    if (!g_live_provider.installed || !args) return false;
    if (!clf4_pla_normalize(args, &sample)) {
        adapter_stats = clf4_pla_stats();
        g_live_provider.live_accept_count = adapter_stats.accept_count;
        g_live_provider.live_reject_count = adapter_stats.reject_count;
        g_live_provider.last_reject_reason = adapter_stats.last_reject_reason;
        return false;
    }

    live_queue_push(&sample.state);
    g_live_provider.submit_count++;
    g_live_provider.has_state = true;
    g_live_provider.last_state = sample.state;
    g_live_provider.last_observed_tick = sample.observed_tick;
    g_live_provider.last_accepted_sequence = sample.accepted_sequence;
    g_live_provider.last_source_kind = sample.source_kind;
    g_live_provider.last_used_default_player_id = sample.used_default_player_id;
    adapter_stats = clf4_pla_stats();
    g_live_provider.live_accept_count = adapter_stats.accept_count;
    g_live_provider.live_reject_count = adapter_stats.reject_count;
    g_live_provider.last_reject_reason = adapter_stats.last_reject_reason;
    return true;
}

LocalPlayerObserverLiveProviderStats lpo_live_provider_stats(void) {
    LocalPlayerObserverLiveProviderStats out;
    memset(&out, 0, sizeof(out));
    out.installed = g_live_provider.installed;
    out.has_state = g_live_provider.has_state;
    out.submit_count = g_live_provider.submit_count;
    out.capture_count = g_live_provider.capture_count;
    out.queued_sample_count = g_live_provider.queue_count;
    out.queue_high_watermark = g_live_provider.queue_high_watermark;
    out.dropped_sample_count = g_live_provider.dropped_sample_count;
    out.live_accept_count = g_live_provider.live_accept_count;
    out.live_reject_count = g_live_provider.live_reject_count;
    out.last_observed_tick = g_live_provider.last_observed_tick;
    out.last_accepted_sequence = g_live_provider.last_accepted_sequence;
    out.last_source_kind = g_live_provider.last_source_kind;
    out.last_used_default_player_id = g_live_provider.last_used_default_player_id;
    out.last_reject_reason = g_live_provider.last_reject_reason;
    out.last_state = g_live_provider.last_state;
    return out;
}

bool lpo_provider_capture(LocalPlayerObservedState* out_state) {
    if (!out_state) return false;
    if (g_provider.capture) return g_provider.capture(g_provider.user, out_state);
    return live_provider_capture(out_state);
}
