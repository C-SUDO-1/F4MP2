#include <assert.h>
#include <string.h>
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "proxy_runtime.h"

int main(void) {
    MsgPlayerState m;
    RemoteProxyUpdatePolicy policy;
    const Fo4SceneProxyPlayerState* s;

    proxy_runtime_init();
    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    memset(&m, 0, sizeof(m));
    m.player_id = 2;
    m.position.x = 0.0f;
    m.rotation.yaw = 0.0f;
    m.state_sequence = 1;
    m.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&m));

    policy = rpup_get();
    policy.enabled = true;
    policy.teleport_distance = 2.0f;
    policy.position_lerp_alpha = 0.5f;
    policy.rotation_lerp_alpha = 0.5f;
    rpup_set(&policy);

    m.position.x = 1.0f;
    m.rotation.yaw = 10.0f;
    m.state_sequence = 2;
    m.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&m));
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s != 0);
    assert(s->last_update_smoothed_position);
    assert(!s->last_update_teleported);
    assert(s->last_update.position.x > 0.4f && s->last_update.position.x < 0.6f);

    m.position.x = 10.0f;
    m.state_sequence = 3;
    m.observed_tick = 3;
    assert(rpm_apply_remote_player_state(&m));
    s = fo4_scene_proxy_backend_stub_get(2);
    assert(s != 0);
    assert(s->last_update_teleported);
    assert(!s->last_update_smoothed_position);
    assert(s->last_update.position.x == 10.0f);

    return 0;
}
