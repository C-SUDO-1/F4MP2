#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_spawn_contract.h"
#include "fo4_proxy_actor_driver.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "messages_player.h"
#include "remote_proxy_manager.h"

int main(void) {
    MsgPlayerState msg;
    Fo4ProxyActorSpawnContractConfig cfg;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;
    Fo4ProxyActorSpawnContractState st;

    memset(&msg, 0, sizeof(msg));
    memset(&cfg, 0, sizeof(cfg));

    fpad_reset_default_driver();
    fpasc_reset();
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x7777u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    fpasc_configure(&cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    msg.player_id = 2;
    msg.position.x = 12.0f;
    msg.rotation.yaw = 25.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    scene = fo4_scene_proxy_backend_stub_get(2);
    driver = fpad_get_state(2);
    st = fpasc_state();
    assert(scene != 0);
    assert(driver != 0);
    assert(st.plan_count == 1);
    assert(st.spawn_count == 1);
    assert(st.last_create_spec.proxy_base_form_id == 0x7777u);
    assert(driver->proxy_base_form_id == 0x7777u);
    assert(driver->position.x == 12.0f);
    return 0;
}
