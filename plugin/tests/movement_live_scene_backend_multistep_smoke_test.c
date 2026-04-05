#include <assert.h>
#include <string.h>

#include "movement_live_client_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"
#include "messages_player.h"

int main(void) {
    MovementLiveClientBridge b;
    MovementLiveClientBridgeConfig cfg;
    Fo4SceneProxyInterpolationConfig icfg;
    RemoteProxyUpdatePolicy pol;
    MsgPlayerState msg;
    const Fo4SceneProxyPlayerState* remote;

    mlcb_init(&b);
    cfg = mlcb_get_config(&b);
    cfg.auto_step_remote_backend = true;
    cfg.remote_backend_step_count = 3u;
    mlcb_set_config(&b, &cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();
    pol.enabled = true;
    pol.teleport_distance = 1000.0f;
    pol.position_lerp_alpha = 1.0f;
    pol.rotation_lerp_alpha = 1.0f;
    rpup_set(&pol);

    icfg.enabled = true;
    icfg.position_alpha = 0.25f;
    icfg.rotation_alpha = 0.25f;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&icfg);

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 0.0f;
    msg.rotation.yaw = 0.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    msg.position.x = 120.0f;
    msg.rotation.yaw = 90.0f;
    msg.state_sequence = 2;
    msg.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&msg));

    mlcb_step_remote_backend(&b);

    remote = fo4_scene_proxy_backend_stub_get(2);
    assert(remote != 0);
    assert(remote->interpolation_tick_count == 3u);
    assert(remote->presented_position.x >= 0.0f);
    assert(remote->presented_position.x <= 120.0f);
    assert(remote->presented_rotation.yaw >= 0.0f);
    assert(remote->presented_rotation.yaw <= 90.0f);
    return 0;
}
