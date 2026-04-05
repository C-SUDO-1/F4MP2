#pragma once

#include <stdbool.h>
#include "service_state_models.h"
#include "messages_player.h"
#include "messages_combat.h"
#include "messages_workshop.h"

bool ac_handle_input_state(SessionState* s, const MsgInputState* msg, PlayerId player_id);
bool ac_handle_fire_intent(SessionState* s, const MsgFireIntent* msg, MsgDamageResult* out_damage, MsgDeathEvent* out_death, bool* out_has_death);
bool ac_handle_build_request(SessionState* s, const MsgBuildRequest* msg, MsgBuildResult* out_result);
bool ac_handle_object_move(SessionState* s, const MsgObjectMoveRequest* msg);
bool ac_handle_object_scrap(SessionState* s, const MsgObjectScrapRequest* msg);
ActorRuntimeState* ac_spawn_hostile_actor(SessionState* s, uint32_t base_form_id, Vec3f position, Rot3f rotation);

void ac_tick(SessionState* s);
