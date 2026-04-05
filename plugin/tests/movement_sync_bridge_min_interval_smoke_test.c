#include <assert.h>
#include <string.h>
#include "movement_sync_bridge.h"
#include "local_player_observer.h"

int main(void) {
    LocalPlayerObservedState s; MsgPlayerState msg; MovementSyncBridgeConfig cfg;
    memset(&s, 0, sizeof(s));
    s.valid = true; s.player_id = 1; s.position.x = 1.0f;
    memset(&cfg, 0, sizeof(cfg));
    cfg.min_send_interval_ticks = 2;
    lpo_init(1);
    lpo_set_synthetic_state(&s);
    msb_init(&cfg);
    assert(msb_build_player_state_if_changed(&msg));
    s.position.x = 2.0f;
    lpo_set_synthetic_state(&s);
    assert(!msb_build_player_state_if_changed(&msg));
    s.position.x = 3.0f;
    lpo_set_synthetic_state(&s);
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.position.x == 3.0f);
    return 0;
}
