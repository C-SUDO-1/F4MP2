#pragma once
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct LocalPlayerObserverStats {
    uint32_t capture_attempts;
    uint32_t provider_hits;
    uint32_t synthetic_hits;
    uint32_t capture_failures;
    bool provider_installed;
    bool last_capture_from_provider;
} LocalPlayerObserverStats;
void lpo_stats_reset(void);
void lpo_stats_note_capture_attempt(void);
void lpo_stats_note_provider_hit(void);
void lpo_stats_note_synthetic_hit(void);
void lpo_stats_note_capture_failure(void);
void lpo_stats_note_provider_installed(bool installed);
LocalPlayerObserverStats lpo_stats_get(void);
#ifdef __cplusplus
}
#endif
