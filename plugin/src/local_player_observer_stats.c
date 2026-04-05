#include "local_player_observer_stats.h"
#include <string.h>
static LocalPlayerObserverStats g_stats;
void lpo_stats_reset(void){ memset(&g_stats,0,sizeof(g_stats)); }
void lpo_stats_note_capture_attempt(void){ g_stats.capture_attempts++; }
void lpo_stats_note_provider_hit(void){ g_stats.provider_hits++; g_stats.last_capture_from_provider = true; }
void lpo_stats_note_synthetic_hit(void){ g_stats.synthetic_hits++; g_stats.last_capture_from_provider = false; }
void lpo_stats_note_capture_failure(void){ g_stats.capture_failures++; }
void lpo_stats_note_provider_installed(bool installed){ g_stats.provider_installed = installed; }
LocalPlayerObserverStats lpo_stats_get(void){ return g_stats; }
