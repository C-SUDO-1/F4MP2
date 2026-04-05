#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_driver.h"

int main(void) {
    Fo4ProxyActorCreateSpec create_spec;
    Fo4ProxyActorMoveSpec move_spec;
    Fo4ProxyActorRotateSpec rotate_spec;
    const Fo4ProxyActorState* state;

    fpad_reset_default_driver();

    memset(&create_spec, 0, sizeof(create_spec));
    create_spec.player_id = 7;
    create_spec.proxy_net_id = 7007;
    create_spec.proxy_base_form_id = 0x1234u;
    create_spec.position.x = 10.0f;
    create_spec.position.y = 20.0f;
    create_spec.rotation.yaw = 45.0f;
    create_spec.equipped_form_id = 0x2222u;
    assert(fpad_create(&create_spec));

    state = fpad_get_state(7);
    assert(state != 0);
    assert(state->visible);
    assert(!state->despawned);
    assert(state->position.x == 10.0f);
    assert(state->rotation.yaw == 45.0f);
    assert(state->last_command == FPAD_CMD_CREATE);
    assert(fpad_create_count() == 1);

    memset(&move_spec, 0, sizeof(move_spec));
    move_spec.player_id = 7;
    move_spec.actor_handle = state->actor_handle;
    move_spec.position.x = 14.0f;
    move_spec.position.y = 25.0f;
    move_spec.velocity.x = 3.0f;
    move_spec.stance = 2;
    move_spec.anim_state = 33;
    move_spec.equipped_form_id = 0x3333u;
    assert(fpad_move(&move_spec));

    state = fpad_get_state(7);
    assert(state != 0);
    assert(state->position.x == 14.0f);
    assert(state->position.y == 25.0f);
    assert(state->velocity.x == 3.0f);
    assert(state->stance == 2);
    assert(state->anim_state == 33);
    assert(state->equipped_form_id == 0x3333u);
    assert(state->last_command == FPAD_CMD_MOVE);
    assert(fpad_move_count() == 1);

    memset(&rotate_spec, 0, sizeof(rotate_spec));
    rotate_spec.player_id = 7;
    rotate_spec.actor_handle = state->actor_handle;
    rotate_spec.rotation.pitch = 1.0f;
    rotate_spec.rotation.yaw = 90.0f;
    rotate_spec.rotation.roll = 2.0f;
    assert(fpad_rotate(&rotate_spec));

    state = fpad_get_state(7);
    assert(state != 0);
    assert(state->rotation.pitch == 1.0f);
    assert(state->rotation.yaw == 90.0f);
    assert(state->rotation.roll == 2.0f);
    assert(state->last_command == FPAD_CMD_ROTATE);
    assert(fpad_rotate_count() == 1);
    assert(fpad_total_command_sequence() == 3);

    assert(fpad_despawn(7));
    state = fpad_get_state(7);
    assert(state != 0);
    assert(!state->visible);
    assert(state->despawned);
    assert(state->last_command == FPAD_CMD_DESPAWN);
    assert(fpad_despawn_count() == 1);
    assert(fpad_total_command_sequence() == 4);

    return 0;
}
