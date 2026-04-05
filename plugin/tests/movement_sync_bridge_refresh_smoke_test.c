#include <assert.h>
#include <string.h>

#include "movement_sync_bridge.h"
#include "local_player_observer.h"

int main(void) {
    MovementSyncBridgeConfig cfg;
    MsgPlayerState msg;
    LocalPlayerObservedState st;

    memset(&cfg, 0, sizeof(cfg));
    cfg.position_epsilon = 0.01f;
    cfg.rotation_epsilon = 0.5f;
    cfg.send_velocity = true;
    cfg.force_resend_ticks = 3;

    lpo_init(1);
    memset(&st, 0, sizeof(st));
    st.valid = true;
    st.player_id = 1;
    st.position.x = 5.0f;
    st.rotation.yaw = 10.0f;
    lpo_set_synthetic_state(&st);

    msb_init(&cfg);
    assert(msb_build_player_state_if_changed(&msg)); /* initial */
    assert(!msb_build_player_state_if_changed(&msg));
    assert(!msb_build_player_state_if_changed(&msg));
    assert(msb_build_player_state_if_changed(&msg)); /* forced resend */
    return 0;
}
