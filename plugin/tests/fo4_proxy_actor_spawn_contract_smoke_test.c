#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_spawn_contract.h"

int main(void) {
    Fo4ProxyActorSpawnContractConfig cfg;
    Fo4ProxyActorSpawnContractState st;
    ProxyPlayerSpawnSpec spawn;
    Fo4ProxyActorCreateSpec create_spec;
    const Fo4ProxyActorState* driver_state;

    fpasc_reset();
    fpad_reset_default_driver();

    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x1337u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    fpasc_configure(&cfg);

    memset(&spawn, 0, sizeof(spawn));
    spawn.player_id = 1;
    spawn.proxy_net_id = 1001;
    spawn.position.x = 4.0f;
    assert(!fpasc_build_create_spec(&spawn, &create_spec));
    st = fpasc_state();
    assert(st.reject_count == 1);
    assert(st.last_suppressed_self_spawn);

    memset(&spawn, 0, sizeof(spawn));
    spawn.player_id = 2;
    spawn.proxy_net_id = 1002;
    spawn.position.x = 9.0f;
    spawn.rotation.yaw = 30.0f;
    spawn.equipped_form_id = 0x2222u;
    assert(fpasc_build_create_spec(&spawn, &create_spec));
    assert(create_spec.player_id == 2);
    assert(create_spec.proxy_base_form_id == 0x1337u);
    assert(create_spec.position.x == 9.0f);
    st = fpasc_state();
    assert(st.plan_count == 1);
    assert(st.last_used_default_base_form);

    assert(fpasc_spawn_via_driver(&spawn, 0xABCDEFu));
    st = fpasc_state();
    assert(st.spawn_count == 1);
    driver_state = fpad_get_state(2);
    assert(driver_state != 0);
    assert(driver_state->visible);
    assert(driver_state->proxy_base_form_id == 0x1337u);
    assert(driver_state->actor_handle == 0xABCDEFu);

    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0;
    cfg.suppress_self_spawn = false;
    cfg.require_base_form_id = true;
    fpasc_configure(&cfg);

    memset(&spawn, 0, sizeof(spawn));
    spawn.player_id = 3;
    spawn.proxy_net_id = 1003;
    assert(!fpasc_build_create_spec(&spawn, &create_spec));
    st = fpasc_state();
    assert(st.reject_count >= 2);
    return 0;
}
