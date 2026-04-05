#pragma once

#include <stdbool.h>
#include "service_state_models.h"

void ws_init_session(SessionState* s, SessionId session_id, uint32_t rules, uint16_t tick_rate);
PlayerRuntimeState* ws_alloc_player(SessionState* s, PlayerId player_id, uint32_t connection_id);
ActorRuntimeState* ws_alloc_actor(SessionState* s, ActorNetId actor_net_id, uint32_t base_form_id, uint8_t actor_kind);
ObjectRuntimeState* ws_alloc_object(SessionState* s, ObjectNetId object_net_id, uint32_t form_id, uint32_t settlement_id);
SettlementState* ws_alloc_settlement(SessionState* s, uint32_t settlement_id, Vec3f center, float radius, uint32_t budget);
void ws_remove_player(SessionState* s, PlayerId player_id);
void ws_remove_actor(SessionState* s, ActorNetId actor_net_id);
void ws_remove_object(SessionState* s, ObjectNetId object_net_id);
