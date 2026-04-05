#include <assert.h>
#include <string.h>

#include "movement_sync_bridge.h"
#include "local_player_observer.h"

static LocalPlayerObservedState make_state(float x, float yaw) {
    LocalPlayerObservedState s;
    memset(&s, 0, sizeof(s));
    s.valid = true;
    s.player_id = 7u;
    s.position.x = x;
    s.rotation.yaw = yaw;
    s.stance = STANCE_IDLE;
    return s;
}

int main(void) {
    MovementSyncBridgeConfig cfg;
    MsgPlayerState msg;
    LocalPlayerObservedState s;

    lpo_init(7u);
    msb_init(NULL);
    cfg = msb_get_config();
    assert(cfg.position_epsilon == 0.02f);
    assert(cfg.rotation_epsilon == 1.0f);
    assert(cfg.force_resend_ticks == 20u);
    assert(cfg.min_send_interval_ticks == 2u);

    s = make_state(0.0f, 0.0f);
    lpo_set_synthetic_state(&s);
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.state_sequence == 1u);

    s = make_state(1.0f, 0.0f); /* changed immediately after first emit: blocked by min interval */
    lpo_set_synthetic_state(&s);
    assert(!msb_build_player_state_if_changed(&msg));

    s = make_state(1.0f, 0.0f);
    lpo_set_synthetic_state(&s);
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.state_sequence == 2u);
    assert(msg.position.x == 1.0f);

    s = make_state(1.01f, 0.0f); /* below position epsilon relative to last emitted state */
    lpo_set_synthetic_state(&s);
    assert(!msb_build_player_state_if_changed(&msg));

    s = make_state(1.0f, 0.0f);
    lpo_set_synthetic_state(&s);
    for (unsigned i = 0; i < 18u; ++i) {
        assert(!msb_build_player_state_if_changed(&msg));
    }
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.state_sequence == 3u);
    assert(msg.observed_tick == msb_last_observed_tick());
    return 0;
}
