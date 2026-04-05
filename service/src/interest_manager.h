#pragma once

#include "service_state_models.h"

void interest_rebuild_bubble(SessionState* s, Vec3f center, float radius, const CellId* cells, uint16_t cell_count);
bool interest_actor_relevant_to_player(const SessionState* s, PlayerId player_id, const ActorRuntimeState* actor);
bool interest_object_relevant_to_player(const SessionState* s, PlayerId player_id, const ObjectRuntimeState* object);
