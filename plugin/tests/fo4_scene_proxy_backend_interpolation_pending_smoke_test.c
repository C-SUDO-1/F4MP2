#include <assert.h>
#include <string.h>

#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"
#include "messages_player.h"

int main(void) {
    MsgPlayerState msg;
    const Fo4SceneProxyPlayerState* s;
    Fo4SceneProxyInterpolationConfig cfg;
    RemoteProxyUpdatePolicy pol;

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();
    pol.enabled = true;
    pol.teleport_distance = 1000.0f;
    pol.position_lerp_alpha = 1.0f;
    pol.rotation_lerp_alpha = 1.0f;
    rpup_set(&pol);

    cfg.enabled = true;
    cfg.position_alpha = 0.5f;
    cfg.rotation_alpha = 0.5f;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&cfg);

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 0.0f;
    msg.rotation.yaw = 0.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    msg.position.x = 10.0f;
    msg.rotation.yaw = 90.0f;
    msg.state_sequence = 2;
    msg.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&msg));

    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s != 0);
    assert(s->interpolation_pending);

    fo4_scene_proxy_backend_stub_step();
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s != 0);
    assert(s->interpolation_pending);

    fo4_scene_proxy_backend_stub_step();
    fo4_scene_proxy_backend_stub_step();
    fo4_scene_proxy_backend_stub_step();
    fo4_scene_proxy_backend_stub_step();
    fo4_scene_proxy_backend_stub_step();
    fo4_scene_proxy_backend_stub_step();
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s != 0);
    assert(!s->interpolation_pending || s->presented_position.x != s->target_position.x);
    return 0;
}
