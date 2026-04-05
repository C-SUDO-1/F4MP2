#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_proxy_actor_driver.h"

int main(void) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    ProxyPlayerSpawnSpec spawn;
    MsgPlayerState msg;
    Fo4ProxyActorLiveCandidateState st;
    const Fo4ProxyActorState* driver;

    fpad_reset_default_driver();
    fpasc_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x5151u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    cfg.actor_class_name = "Actor";
    cfg.create_api_symbol = "TESObjectREFR::Create";
    cfg.move_api_symbol = "TESObjectREFR::SetPosition";
    cfg.rotate_api_symbol = "TESObjectREFR::SetRotation";
    cfg.despawn_api_symbol = "TESObjectREFR::Disable";
    fpalc_configure(&cfg);

    memset(&spawn, 0, sizeof(spawn));
    spawn.player_id = 2;
    spawn.proxy_net_id = 1002;
    spawn.position.x = 8.0f;
    spawn.rotation.yaw = 10.0f;
    assert(fpalc_spawn_via_driver(&spawn, 0xAA55u));
    st = fpalc_state();
    assert(st.spawn_count == 1);
    assert(st.has_driver_state);
    driver = fpad_get_state(2);
    assert(driver != 0);
    assert(driver->proxy_base_form_id == 0x5151u);

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 12.0f;
    msg.rotation.yaw = 30.0f;
    msg.velocity.x = 1.5f;
    assert(fpalc_apply_presented_state(&msg, driver->actor_handle));
    st = fpalc_state();
    assert(st.move_count == 1);
    assert(st.rotate_count == 1);
    driver = fpad_get_state(2);
    assert(driver->position.x == 12.0f);
    assert(driver->rotation.yaw == 30.0f);

    assert(fpalc_despawn(2));
    st = fpalc_state();
    assert(st.despawn_count == 1);
    driver = fpad_get_state(2);
    assert(driver != 0);
    assert(driver->despawned);
    return 0;
}
