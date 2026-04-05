#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "local_player_observer.h"
#include "movement_sync_bridge.h"
#include "protocol_apply.h"
#include "proxy_runtime.h"
#include "remote_proxy_manager.h"

int main(void) {
    LocalPlayerObservedState s;
    MsgPlayerState msg;
    MovementSyncBridgeConfig cfg;
    uint8_t packet[512];
    size_t n;
    const ProxyPlayerRecord* rec;

    proxy_runtime_init();
    rpm_init(1);
    lpo_init(1);
    memset(&cfg, 0, sizeof(cfg));
    cfg.position_epsilon = 0.01f;
    cfg.rotation_epsilon = 0.1f;
    cfg.send_velocity = true;
    msb_init(&cfg);

    memset(&s, 0, sizeof(s));
    s.valid = true;
    s.player_id = 1;
    s.position.x = 5.0f;
    s.position.y = 6.0f;
    s.rotation.yaw = 45.0f;
    s.velocity.x = 1.0f;
    s.stance = STANCE_WALK;
    lpo_set_synthetic_state(&s);

    assert(msb_build_player_state_if_changed(&msg));
    msg.player_id = 2; /* simulate remote echoed player */
    n = msb_encode_player_state_packet(packet, sizeof(packet), 100, 1, &msg);
    assert(n > 0);
    assert(plugin_apply_packet(packet, n));

    rec = proxy_runtime_get_player(2);
    assert(rec != 0);
    assert(rec->position.x == 5.0f);
    assert(rec->position.y == 6.0f);
    assert(rec->rotation.yaw == 45.0f);
    assert(rec->velocity.x == 1.0f);
    assert(rec->stance == STANCE_WALK);

    /* unchanged state should not emit */
    assert(!msb_build_player_state_if_changed(&msg));

    return 0;
}
