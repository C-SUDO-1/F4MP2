#include "world_state_store.h"
#include "service_indices.h"
#include <string.h>
#include "world_trace_provider.h"

void ws_init_session(SessionState* s, SessionId session_id, uint32_t rules, uint16_t tick_rate) {
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->session_id = session_id;
    s->rules = rules;
    s->tick_rate = tick_rate;
    wtp_init(&s->trace_provider);
}

PlayerRuntimeState* ws_alloc_player(SessionState* s, PlayerId player_id, uint32_t connection_id) {
    size_t i;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (!s->players[i].connected) {
            memset(&s->players[i], 0, sizeof(s->players[i]));
            s->players[i].connected = true;
            s->players[i].player_id = player_id;
            s->players[i].connection_id = connection_id;
            s->connected_count++;
            return &s->players[i];
        }
    }
    return NULL;
}

ActorRuntimeState* ws_alloc_actor(SessionState* s, ActorNetId actor_net_id, uint32_t base_form_id, uint8_t actor_kind) {
    size_t i;
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
        if (!s->actors[i].in_use) {
            memset(&s->actors[i], 0, sizeof(s->actors[i]));
            s->actors[i].in_use = true;
            s->actors[i].actor_net_id = actor_net_id;
            s->actors[i].base_form_id = base_form_id;
            s->actors[i].actor_kind = actor_kind;
            s->actors[i].authority_owned_by_host = true;
            return &s->actors[i];
        }
    }
    return NULL;
}

ObjectRuntimeState* ws_alloc_object(SessionState* s, ObjectNetId object_net_id, uint32_t form_id, uint32_t settlement_id) {
    size_t i;
    for (i = 0; i < F4MP_MAX_OBJECTS_PER_SESSION; ++i) {
        if (!s->objects[i].in_use) {
            memset(&s->objects[i], 0, sizeof(s->objects[i]));
            s->objects[i].in_use = true;
            s->objects[i].object_net_id = object_net_id;
            s->objects[i].form_id = form_id;
            s->objects[i].settlement_id = settlement_id;
            return &s->objects[i];
        }
    }
    return NULL;
}

SettlementState* ws_alloc_settlement(SessionState* s, uint32_t settlement_id, Vec3f center, float radius, uint32_t budget) {
    size_t i;
    for (i = 0; i < F4MP_MAX_SETTLEMENTS; ++i) {
        if (!s->settlements[i].in_use) {
            memset(&s->settlements[i], 0, sizeof(s->settlements[i]));
            s->settlements[i].in_use = true;
            s->settlements[i].settlement_id = settlement_id;
            s->settlements[i].center = center;
            s->settlements[i].allowed_radius = radius;
            s->settlements[i].object_budget = budget;
            return &s->settlements[i];
        }
    }
    return NULL;
}

void ws_remove_player(SessionState* s, PlayerId player_id) {
    PlayerRuntimeState* p = find_player(s, player_id);
    if (!p) return;
    memset(p, 0, sizeof(*p));
    if (s->connected_count > 0) s->connected_count--;
}

void ws_remove_actor(SessionState* s, ActorNetId actor_net_id) {
    ActorRuntimeState* a = find_actor(s, actor_net_id);
    if (!a) return;
    memset(a, 0, sizeof(*a));
}

void ws_remove_object(SessionState* s, ObjectNetId object_net_id) {
    ObjectRuntimeState* o = find_object(s, object_net_id);
    if (!o) return;
    memset(o, 0, sizeof(*o));
}
