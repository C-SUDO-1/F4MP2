#include "session_manager.h"
#include "world_state_store.h"
#include "service_indices.h"
#include "authority_core.h"
#include "workshop_whitelist.h"
#include <string.h>

#define F4MP_MAX_SESSIONS 16
static SessionState g_sessions[F4MP_MAX_SESSIONS];

void sm_init(void) { memset(g_sessions, 0, sizeof(g_sessions)); workshop_whitelist_init_default(); }

SessionState* sm_create_session(SessionId session_id, uint32_t rules, uint16_t tick_rate, PlayerId host_player_id) {
    size_t i;
    for (i = 0; i < F4MP_MAX_SESSIONS; ++i) {
        if (!g_sessions[i].in_use) {
            ws_init_session(&g_sessions[i], session_id, rules, tick_rate);
            g_sessions[i].host_player_id = host_player_id;
            return &g_sessions[i];
        }
    }
    return NULL;
}

SessionState* sm_get_session(SessionId session_id) {
    size_t i;
    for (i = 0; i < F4MP_MAX_SESSIONS; ++i) if (g_sessions[i].in_use && g_sessions[i].session_id == session_id) return &g_sessions[i];
    return NULL;
}

bool sm_join_session(SessionState* s, PlayerId player_id, uint32_t connection_id) {
    return ws_alloc_player(s, player_id, connection_id) != NULL;
}

bool sm_remove_player(SessionState* s, PlayerId player_id) {
    size_t i;
    if (!s) return false;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (s->players[i].player_id == player_id) {
            s->players[i].connected = false;
            s->players[i].connection_id = 0;
            return true;
        }
    }
    return false;
}

bool sm_reconnect_player(SessionState* s, PlayerId player_id, uint32_t connection_id) {
    size_t i;
    if (!s) return false;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (s->players[i].player_id == player_id) {
            s->players[i].connected = true;
            s->players[i].connection_id = connection_id;
            return true;
        }
    }
    return false;
}

bool sm_spawn_default_hostile(SessionState* s, uint32_t base_form_id, Vec3f position) {
    Rot3f rot = {0};
    return ac_spawn_hostile_actor(s, base_form_id, position, rot) != NULL;
}
