#pragma once

#include "service_state_models.h"

#ifdef __cplusplus
extern "C" {
#endif

const PlayerStateRelayStats* psrr_get_player_stats(const SessionState* s, PlayerId player_id);

#ifdef __cplusplus
}
#endif
