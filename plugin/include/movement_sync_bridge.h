#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "local_player_observer.h"
#include "messages_player.h"
#include "packet_header.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MovementSyncBridgeConfig {
    float position_epsilon;
    float rotation_epsilon;
    bool  send_velocity;
    uint32_t force_resend_ticks;
    uint32_t min_send_interval_ticks;
} MovementSyncBridgeConfig;

void msb_init(const MovementSyncBridgeConfig* cfg);
MovementSyncBridgeConfig msb_get_config(void);
bool msb_build_player_state_if_changed(MsgPlayerState* out_msg);
uint32_t msb_last_state_sequence(void);
uint32_t msb_last_observed_tick(void);
size_t msb_encode_player_state_packet(
    uint8_t* buffer,
    size_t capacity,
    uint32_t session_id,
    uint32_t sequence,
    const MsgPlayerState* msg
);

#ifdef __cplusplus
}
#endif
