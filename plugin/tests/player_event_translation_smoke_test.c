#include <assert.h>
#include <string.h>
#include "player_event_translation.h"

int main(void) {
    PlayerHostEvent ev;
    MsgPlayerState msg;
    memset(&ev, 0, sizeof(ev));
    memset(&msg, 0, sizeof(msg));
    ev.player_id = 2;
    ev.position.x = 10.0f;
    ev.rotation.yaw = 45.0f;
    ev.velocity.z = 1.0f;
    ev.stance = STANCE_RUN;
    ev.anim_state = 7;
    ev.equipped_form_id = 0x1234u;
    assert(phe_translate_player_state(&ev, &msg));
    assert(msg.player_id == 2);
    assert(msg.position.x == 10.0f);
    assert(msg.rotation.yaw == 45.0f);
    assert(msg.velocity.z == 1.0f);
    assert(msg.stance == STANCE_RUN);
    assert(msg.anim_state == 7);
    assert(msg.equipped_form_id == 0x1234u);
    return 0;
}
