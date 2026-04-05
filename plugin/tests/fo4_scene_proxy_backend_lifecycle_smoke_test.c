#include <assert.h>
#include <string.h>
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "messages_player.h"

int main(void) {
    MsgPlayerState msg;
    const Fo4SceneProxyPlayerState* s;
    memset(&msg, 0, sizeof(msg));

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    msg.player_id = 2;
    msg.position.x = 10.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s);
    assert(s->last_command == FSP_CMD_UPDATE);
    assert(s->command_sequence == 2); /* spawn then update */

    msg.position.x = 12.0f;
    msg.state_sequence = 2;
    msg.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&msg));
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s);
    assert(s->last_command == FSP_CMD_UPDATE);
    assert(s->command_sequence == 3);

    assert(rpm_remove_remote_player(2));
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s);
    assert(s->last_command == FSP_CMD_DESPAWN);
    assert(s->despawned);
    assert(fo4_scene_proxy_backend_stub_total_command_sequence() == 4);
    return 0;
}
