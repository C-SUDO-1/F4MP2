#include "local_player_observer.h"
#include "local_player_observer_provider.h"
#include "local_player_observer_stats.h"
#include <string.h>

static LocalPlayerObservedState g_state;
static PlayerId g_local_player_id = 0;

void lpo_init(PlayerId local_player_id) {
    memset(&g_state, 0, sizeof(g_state));
    g_local_player_id = local_player_id;
    g_state.player_id = local_player_id;
    lpo_stats_reset();
    lpo_stats_note_provider_installed(lpo_has_provider());
}

void lpo_set_synthetic_state(const LocalPlayerObservedState* state) {
    if (!state) return;
    g_state = *state;
    g_state.valid = true;
    if (g_state.player_id == 0) g_state.player_id = g_local_player_id;
}

extern bool lpo_provider_capture(LocalPlayerObservedState* out_state);

bool lpo_capture(LocalPlayerObservedState* out_state) {
    if (!out_state) return false;
    lpo_stats_note_capture_attempt();
    lpo_stats_note_provider_installed(lpo_has_provider());
    if (lpo_has_provider() && lpo_provider_capture(out_state)) {
        if (out_state->player_id == 0) out_state->player_id = g_local_player_id;
        out_state->valid = true;
        lpo_stats_note_provider_hit();
        return true;
    }
    if (!g_state.valid) { lpo_stats_note_capture_failure(); return false; }
    *out_state = g_state;
    lpo_stats_note_synthetic_hit();
    return true;
}

PlayerId lpo_local_player_id(void) {
    return g_local_player_id;
}
