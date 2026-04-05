#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PlayerStateRelayDecision {
    PSR_REJECT = 0,
    PSR_ACCEPT = 1
} PlayerStateRelayDecision;

typedef struct PlayerStateRelayStats {
    uint32_t accepted_count;
    uint32_t rejected_stale_count;
    uint32_t rejected_regressive_tick_count;
    uint32_t last_sequence;
    uint32_t last_observed_tick;
} PlayerStateRelayStats;

void psr_init(PlayerStateRelayStats* s);
PlayerStateRelayDecision psr_evaluate(PlayerStateRelayStats* s, const MsgPlayerState* msg);

#ifdef __cplusplus
}
#endif
