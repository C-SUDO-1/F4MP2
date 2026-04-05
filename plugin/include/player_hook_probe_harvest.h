#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "hook_discovery_report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlayerHookProbeHarvestSnapshot {
    uint32_t session_id;
    bool active;
    const char* family;
    const char* reason;
    const char* mode;
    uint32_t sample_count;
    uint32_t accepted_sample_count;
    uint32_t synthetic_sample_count;
    uint32_t cluster_count;
    uint32_t promoted_candidate_count;
} PlayerHookProbeHarvestSnapshot;

void phh_reset(void);
void phh_begin_unresolved_session(const HookDiscoveryReport* report);
void phh_note_reachable_seam(const char* seam_name,
                            bool synthetic_event,
                            bool local_player_valid,
                            bool movement_context_valid);
void phh_note_explicit_callsite(const char* seam_name,
                                uintptr_t return_addr,
                                uintptr_t caller_addr,
                                bool synthetic_event,
                                bool local_player_valid,
                                bool movement_context_valid);
PlayerHookProbeHarvestSnapshot phh_snapshot(void);
void phh_refresh_report(HookDiscoveryReport* report);

#ifdef __cplusplus
}
#endif
