#include <assert.h>
#include <math.h>
#include "movement_live_client_bridge.h"

int main(void) {
    MovementLiveClientBridge b;
    MovementLiveClientBridgeConfig cfg;
    MovementLiveClientBridgeConfig got;
    mlcb_init(&b);
    cfg = mlcb_get_config(&b);
    cfg.heartbeat_interval_ticks = 17;
    cfg.auto_step_remote_backend = false;
    cfg.remote_backend_step_count = 3;
    cfg.apply_scene_backend_interp_config = true;
    cfg.scene_interp_config.enabled = false;
    cfg.scene_interp_config.position_alpha = 0.25f;
    cfg.scene_interp_config.rotation_alpha = 0.75f;
    mlcb_set_config(&b, &cfg);
    got = mlcb_get_config(&b);
    assert(got.heartbeat_interval_ticks == 17);
    assert(got.auto_step_remote_backend == false);
    assert(got.remote_backend_step_count == 3);
    assert(got.apply_scene_backend_interp_config == true);
    assert(got.scene_interp_config.enabled == false);
    assert(fabsf(got.scene_interp_config.position_alpha - 0.25f) < 0.0001f);
    assert(fabsf(got.scene_interp_config.rotation_alpha - 0.75f) < 0.0001f);
    return 0;
}
