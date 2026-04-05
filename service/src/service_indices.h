#pragma once

#include <stddef.h>
#include "service_state_models.h"

PlayerRuntimeState* find_player(SessionState* s, PlayerId player_id);
const PlayerRuntimeState* find_player_const(const SessionState* s, PlayerId player_id);
ActorRuntimeState* find_actor(SessionState* s, ActorNetId actor_net_id);
const ActorRuntimeState* find_actor_const(const SessionState* s, ActorNetId actor_net_id);
ObjectRuntimeState* find_object(SessionState* s, ObjectNetId object_net_id);
const ObjectRuntimeState* find_object_const(const SessionState* s, ObjectNetId object_net_id);
SettlementState* find_settlement(SessionState* s, uint32_t settlement_id);
const SettlementState* find_settlement_const(const SessionState* s, uint32_t settlement_id);
