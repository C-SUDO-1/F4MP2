#include "player_event_translation.h"
#include <string.h>

bool phe_translate_player_state(const PlayerHostEvent* ev, MsgPlayerState* out_msg) {
    if (!ev || !out_msg) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->player_id = ev->player_id;
    out_msg->position = ev->position;
    out_msg->rotation = ev->rotation;
    out_msg->velocity = ev->velocity;
    out_msg->stance = ev->stance;
    out_msg->anim_state = ev->anim_state;
    out_msg->equipped_form_id = ev->equipped_form_id;
    return true;
}
