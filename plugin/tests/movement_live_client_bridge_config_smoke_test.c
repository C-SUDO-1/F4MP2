#include <assert.h>
#include "movement_live_client_bridge.h"

int main(void) {
    MovementLiveClientBridge b;
    MovementLiveClientBridgeConfig cfg;
    mlcb_init(&b);

    cfg = mlcb_get_config(&b);
    assert(cfg.heartbeat_interval_ticks == 10u);
    assert(cfg.auto_step_remote_backend == true);
    assert(cfg.remote_backend_step_count == 2u);

    cfg.heartbeat_interval_ticks = 3u;
    cfg.auto_step_remote_backend = false;
    cfg.remote_backend_step_count = 5u;
    mlcb_set_config(&b, &cfg);

    cfg = mlcb_get_config(&b);
    assert(cfg.heartbeat_interval_ticks == 3u);
    assert(cfg.auto_step_remote_backend == false);
    assert(cfg.remote_backend_step_count == 5u);
    return 0;
}
