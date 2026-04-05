#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_player.h"

typedef struct PlayerHostEvent {
    PlayerId player_id;
    Vec3f position;
    Rot3f rotation;
    Vec3f velocity;
    uint8_t stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
} PlayerHostEvent;

bool phe_translate_player_state(const PlayerHostEvent* ev, MsgPlayerState* out_msg);
