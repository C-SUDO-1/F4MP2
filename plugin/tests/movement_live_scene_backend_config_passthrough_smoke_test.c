#include <assert.h>
#include <math.h>
#include "movement_live_client_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"

int main(void) {
    MovementLiveClientBridge b;
    MovementLiveClientBridgeConfig cfg;
    Fo4SceneProxyInterpolationConfig got;
    mlcb_init(&b);
    cfg = mlcb_get_config(&b);
    cfg.apply_scene_backend_interp_config = true;
    cfg.scene_interp_config.enabled = true;
    cfg.scene_interp_config.position_alpha = 0.1f;
    cfg.scene_interp_config.rotation_alpha = 0.2f;
    mlcb_set_config(&b, &cfg);
    got = fo4_scene_proxy_backend_stub_get_interpolation_config();
    assert(fabsf(got.position_alpha - 0.1f) < 0.0001f);
    assert(fabsf(got.rotation_alpha - 0.2f) < 0.0001f);
    assert(got.enabled == true);
    return 0;
}
