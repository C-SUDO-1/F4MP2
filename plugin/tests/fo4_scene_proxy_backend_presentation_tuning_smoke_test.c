#include <assert.h>
#include <math.h>
#include <string.h>

#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "remote_proxy_backend.h"
#include "remote_proxy_update_policy.h"

static float absf_local(float v) { return v < 0.0f ? -v : v; }

int main(void) {
    RemoteProxyUpdatePolicy pol;
    Fo4SceneProxyInterpolationConfig interp;
    const RemoteProxyBackend* backend;
    ProxyPlayerSpawnSpec spawn;
    ProxyPlayerUpdateSpec update;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;

    rpup_init_default();
    pol = rpup_get();
    assert(pol.teleport_distance == 8.0f);
    assert(pol.position_lerp_alpha == 0.25f);
    assert(pol.rotation_lerp_alpha == 0.35f);

    fo4_scene_proxy_backend_stub_reset();
    interp = fo4_scene_proxy_backend_stub_get_interpolation_config();
    assert(interp.enabled);
    assert(interp.position_alpha == 0.25f);
    assert(interp.rotation_alpha == 0.35f);

    fpad_reset_default_driver();
    fo4_scene_proxy_backend_stub_install();
    backend = rpb_get_backend();
    assert(backend != 0);
    assert(backend->spawn_player != 0);
    assert(backend->update_player != 0);

    memset(&spawn, 0, sizeof(spawn));
    spawn.player_id = 9u;
    spawn.proxy_net_id = 9u;
    spawn.position.x = 0.0f;
    spawn.rotation.yaw = 0.0f;
    assert(backend->spawn_player(backend->user, &spawn));

    memset(&update, 0, sizeof(update));
    update.player_id = 9u;
    update.position.x = 100.0f;
    update.rotation.yaw = 100.0f;
    assert(backend->update_player(backend->user, &update));
    fo4_scene_proxy_backend_stub_note_policy_result(9u, false, true, true, 100.0f);

    scene = fo4_scene_proxy_backend_stub_get(9u);
    driver = fpad_get_state(9u);
    assert(scene != 0);
    assert(driver != 0);
    assert(scene->presented_position.x == 0.0f);
    assert(scene->target_position.x == 100.0f);
    assert(driver->position.x == 0.0f);

    fo4_scene_proxy_backend_stub_step();
    scene = fo4_scene_proxy_backend_stub_get(9u);
    driver = fpad_get_state(9u);
    assert(absf_local(scene->presented_position.x - 25.0f) < 0.001f);
    assert(absf_local(scene->presented_rotation.yaw - 35.0f) < 0.001f);
    assert(absf_local(driver->position.x - 25.0f) < 0.001f);
    assert(absf_local(driver->rotation.yaw - 35.0f) < 0.001f);

    fo4_scene_proxy_backend_stub_step();
    scene = fo4_scene_proxy_backend_stub_get(9u);
    driver = fpad_get_state(9u);
    assert(absf_local(scene->presented_position.x - 43.75f) < 0.001f);
    assert(absf_local(scene->presented_rotation.yaw - 57.75f) < 0.001f);
    assert(absf_local(driver->position.x - 43.75f) < 0.001f);
    assert(absf_local(driver->rotation.yaw - 57.75f) < 0.001f);
    return 0;
}
