#include <assert.h>
#include "proxy_runtime.h"
#include "plugin_safety_contract.h"

int main(void) {
    ProxyPlayerSpawnSpec ps = {0};
    ProxyPlayerUpdateSpec pu = {0};
    ProxyActorSpawnSpec as = {0};
    ProxyActorUpdateSpec au = {0};
    proxy_runtime_init();

    ps.player_id = 2; ps.proxy_net_id = 2002; ps.proxy_base_form_id = 0x1000; ps.position.x = 10.0f;
    assert(proxy_spawn_player(&ps));
    assert(proxy_runtime_player_count() == 1);
    pu.player_id = 2; pu.position.y = 5.0f; pu.anim_state = 7;
    assert(proxy_update_player(&pu));

    as.actor_net_id = 3001; as.base_form_id = 0x2000; as.actor_kind = 1;
    assert(proxy_spawn_actor(&as));
    assert(proxy_runtime_actor_count() == 1);
    au.actor_net_id = 3001; au.health_norm = 0.5f;
    assert(proxy_update_actor(&au));

    assert(safety_can_guest_mutate_world(2).action == SAFETY_HOST_ONLY);
    assert(safety_can_guest_place_object(2, 0xAAAAAAAAu).action == SAFETY_ALLOW);

    assert(proxy_despawn_actor(3001));
    assert(proxy_despawn_player(2));
    return 0;
}
