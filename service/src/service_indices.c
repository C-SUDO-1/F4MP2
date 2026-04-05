#include "service_indices.h"

PlayerRuntimeState* find_player(SessionState* s, PlayerId player_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (s->players[i].connected && s->players[i].player_id == player_id) return &s->players[i];
    return NULL;
}
const PlayerRuntimeState* find_player_const(const SessionState* s, PlayerId player_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (s->players[i].connected && s->players[i].player_id == player_id) return &s->players[i];
    return NULL;
}
ActorRuntimeState* find_actor(SessionState* s, ActorNetId actor_net_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) if (s->actors[i].in_use && s->actors[i].actor_net_id == actor_net_id) return &s->actors[i];
    return NULL;
}
const ActorRuntimeState* find_actor_const(const SessionState* s, ActorNetId actor_net_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) if (s->actors[i].in_use && s->actors[i].actor_net_id == actor_net_id) return &s->actors[i];
    return NULL;
}
ObjectRuntimeState* find_object(SessionState* s, ObjectNetId object_net_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_OBJECTS_PER_SESSION; ++i) if (s->objects[i].in_use && s->objects[i].object_net_id == object_net_id) return &s->objects[i];
    return NULL;
}
const ObjectRuntimeState* find_object_const(const SessionState* s, ObjectNetId object_net_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_OBJECTS_PER_SESSION; ++i) if (s->objects[i].in_use && s->objects[i].object_net_id == object_net_id) return &s->objects[i];
    return NULL;
}
SettlementState* find_settlement(SessionState* s, uint32_t settlement_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_SETTLEMENTS; ++i) if (s->settlements[i].in_use && s->settlements[i].settlement_id == settlement_id) return &s->settlements[i];
    return NULL;
}
const SettlementState* find_settlement_const(const SessionState* s, uint32_t settlement_id) {
    size_t i; if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_SETTLEMENTS; ++i) if (s->settlements[i].in_use && s->settlements[i].settlement_id == settlement_id) return &s->settlements[i];
    return NULL;
}
