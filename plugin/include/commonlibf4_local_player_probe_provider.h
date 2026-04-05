#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_player_hook_runtime_probe.h"
#include "local_player_observer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4LocalPlayerProbeProviderConfig {
    PlayerId expected_local_player_id;
    bool require_hook_correct;
} CommonLibF4LocalPlayerProbeProviderConfig;

typedef struct CommonLibF4LocalPlayerProbeProviderStats {
    bool installed;
    bool has_state;
    bool require_hook_correct;
    uint32_t capture_count;
    uint32_t stale_capture_count;
    uint32_t blocked_capture_count;
    uint32_t last_callback_count;
    PlayerId expected_local_player_id;
    LocalPlayerObservedState last_state;
} CommonLibF4LocalPlayerProbeProviderStats;

void clf4_lppp_install(const CommonLibF4LocalPlayerProbeProviderConfig* config);
void clf4_lppp_uninstall(void);
bool clf4_lppp_installed(void);
CommonLibF4LocalPlayerProbeProviderStats clf4_lppp_stats(void);

#ifdef __cplusplus
}
#endif
