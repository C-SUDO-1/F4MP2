#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "commonlibf4_player_live_adapter.h"
#include "local_player_observer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LocalPlayerObserverProvider {
    void* user;
    bool (*capture)(void* user, LocalPlayerObservedState* out_state);
} LocalPlayerObserverProvider;

typedef struct LocalPlayerObserverLiveProviderStats {
    bool installed;
    bool has_state;
    uint32_t submit_count;
    uint32_t capture_count;
    uint32_t queued_sample_count;
    uint32_t queue_high_watermark;
    uint32_t dropped_sample_count;
    uint32_t live_accept_count;
    uint32_t live_reject_count;
    uint32_t last_observed_tick;
    uint32_t last_accepted_sequence;
    uint8_t last_source_kind;
    bool last_used_default_player_id;
    CommonLibF4PlayerLiveRejectReason last_reject_reason;
    LocalPlayerObservedState last_state;
} LocalPlayerObserverLiveProviderStats;

void lpo_install_provider(const LocalPlayerObserverProvider* provider);
void lpo_clear_provider(void);
bool lpo_has_provider(void);

void lpo_live_provider_install(PlayerId fallback_player_id);
void lpo_live_provider_uninstall(void);
bool lpo_live_provider_installed(void);
bool lpo_live_provider_submit(const CommonLibF4PlayerHookArgs* args);
LocalPlayerObserverLiveProviderStats lpo_live_provider_stats(void);

#ifdef __cplusplus
}
#endif
