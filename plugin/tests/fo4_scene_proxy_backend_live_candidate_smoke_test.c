#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "messages_player.h"

int main(void) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    MsgPlayerState msg;
    Fo4ProxyActorLiveCandidateState st;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;

    fpad_reset_default_driver();
    fpasc_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x6262u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    fpalc_configure(&cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 5.0f;
    msg.rotation.yaw = 15.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));

    st = fpalc_state();
    scene = fo4_scene_proxy_backend_stub_get(2);
    driver = fpad_get_state(2);
    assert(scene != 0);
    assert(driver != 0);
    assert(st.spawn_count == 1);
    assert(st.move_count >= 1);
    assert(st.rotate_count >= 1);
    assert(driver->proxy_base_form_id == 0x6262u);
    assert(driver->position.x == 5.0f);
    return 0;
}
