#pragma once

#include <stdbool.h>
#include "service_state_models.h"

void sm_init(void);
SessionState* sm_create_session(SessionId session_id, uint32_t rules, uint16_t tick_rate, PlayerId host_player_id);
SessionState* sm_get_session(SessionId session_id);
bool sm_join_session(SessionState* s, PlayerId player_id, uint32_t connection_id);
bool sm_remove_player(SessionState* s, PlayerId player_id);
bool sm_reconnect_player(SessionState* s, PlayerId player_id, uint32_t connection_id);

bool sm_spawn_default_hostile(SessionState* s, uint32_t base_form_id, Vec3f position);
