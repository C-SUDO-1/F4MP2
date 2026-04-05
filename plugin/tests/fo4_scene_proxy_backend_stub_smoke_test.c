#include <assert.h>
#include <string.h>

#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"

int main(void) {
    MsgPlayerState msg;
    const Fo4SceneProxyPlayerState* remote;

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 10.0f;
    msg.position.y = 20.0f;
    msg.rotation.yaw = 45.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    remote = fo4_scene_proxy_backend_stub_get(2);
    assert(remote != 0);
    assert(remote->visible);
    assert(remote->actor_handle != 0);
    assert(remote->last_update.position.x == 10.0f);
    assert(remote->transform_apply_count >= 1);

    assert(rpm_remove_remote_player(2));
    remote = fo4_scene_proxy_backend_stub_get(2);
    assert(remote != 0);
    assert(remote->despawned);
    assert(!remote->visible);
    return 0;
}
