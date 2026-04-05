#pragma once

#include <stdbool.h>
#include "service_state_models.h"
#include "messages_workshop.h"
#include "messages_combat.h"
#include "combat_geometry.h"

bool validate_player_connected(const SessionState* s, PlayerId player_id);
bool validate_player_inside_bubble(const SessionState* s, PlayerId player_id);
bool validate_build_request(const SessionState* s, const MsgBuildRequest* req);
bool validate_fire_intent(const SessionState* s, const MsgFireIntent* req);
bool validate_object_move_request(const SessionState* s, const MsgObjectMoveRequest* req);
bool validate_object_scrap_request(const SessionState* s, const MsgObjectScrapRequest* req);

CombatGeometryConfig av_default_combat_geometry(void);
