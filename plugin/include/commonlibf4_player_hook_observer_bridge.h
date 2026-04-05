#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "commonlibf4_hook_adapter_stub.h"
#include "commonlibf4_player_live_adapter.h"
#include "local_player_observer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLF4_PHOB_MAX_QUEUE 8

typedef struct CommonLibF4PlayerHookObserverBridgeStats {
    uint32_t submit_count;
    uint32_t capture_count;
    uint32_t queued_sample_count;
    uint32_t queue_high_watermark;
    uint32_t dropped_sample_count;
    uint32_t live_accept_count;
    uint32_t live_reject_count;
    uint32_t last_observed_tick;
    uint8_t last_source_kind;
    bool last_used_default_player_id;
    bool provider_installed;
    bool has_state;
    LocalPlayerObservedState last_state;
} CommonLibF4PlayerHookObserverBridgeStats;

void clf4_phob_install(PlayerId local_player_id);
void clf4_phob_uninstall(void);
bool clf4_phob_submit(const CommonLibF4PlayerHookArgs* args);
CommonLibF4PlayerHookObserverBridgeStats clf4_phob_stats(void);

#ifdef __cplusplus
}
#endif
