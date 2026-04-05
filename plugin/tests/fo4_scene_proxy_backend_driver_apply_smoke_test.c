#include <assert.h>
#include <math.h>
#include <string.h>

#include "fo4_proxy_actor_driver.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "messages_player.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"

int main(void) {
    MsgPlayerState msg;
    Fo4SceneProxyInterpolationConfig cfg;
    RemoteProxyUpdatePolicy pol;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;

    memset(&msg, 0, sizeof(msg));
    fpad_reset_default_driver();
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

    msg.player_id = 2;
    msg.position.x = 0.0f;
    msg.rotation.yaw = 0.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    driver = fpad_get_state(2);
    assert(driver != 0);
    assert(driver->create_count == 1);
    assert(driver->move_count == 1);
    assert(driver->rotate_count == 1);
    assert(driver->position.x == 0.0f);
    assert(driver->rotation.yaw == 0.0f);

    msg.position.x = 20.0f;
    msg.rotation.yaw = 90.0f;
    msg.state_sequence = 2;
    msg.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&msg));

    scene = fo4_scene_proxy_backend_stub_get(2);
    driver = fpad_get_state(2);
    assert(scene != 0);
    assert(driver != 0);
    assert(scene->target_position.x == 20.0f);
    assert(scene->presented_position.x == 0.0f);
    assert(driver->position.x == 0.0f);
    assert(driver->rotation.yaw == 0.0f);

    fo4_scene_proxy_backend_stub_step();

    scene = fo4_scene_proxy_backend_stub_get(2);
    driver = fpad_get_state(2);
    assert(scene != 0);
    assert(driver != 0);
    assert(scene->interpolation_tick_count == 1);
    assert(fabsf(scene->presented_position.x - 10.0f) < 0.001f);
    assert(fabsf(driver->position.x - 10.0f) < 0.001f);
    assert(fabsf(driver->rotation.yaw - 45.0f) < 0.001f);
    assert(driver->move_count >= 3);
    assert(driver->rotate_count >= 3);

    assert(rpm_remove_remote_player(2));
    driver = fpad_get_state(2);
    assert(driver != 0);
    assert(driver->despawned);
    assert(driver->despawn_count == 1);
    return 0;
}
