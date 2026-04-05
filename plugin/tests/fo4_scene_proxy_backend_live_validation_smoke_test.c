#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_proxy_actor_live_validation.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"

int main(void) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    Fo4SceneProxyInterpolationConfig interp;
    MsgPlayerState msg;
    Fo4ProxyActorLiveValidationResult result;
    const Fo4SceneProxyPlayerState* scene;

    fpad_reset_default_driver();
    fpasc_reset();
    fpalv_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x8484u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    fpalc_configure(&cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();
    memset(&interp, 0, sizeof(interp));
    interp.enabled = true;
    interp.position_alpha = 0.5f;
    interp.rotation_alpha = 0.5f;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&interp);

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 4.0f;
    msg.rotation.yaw = 10.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));
    assert(fpalv_validate_present(2, &result));

    msg.position.x = 8.0f;
    msg.rotation.yaw = 20.0f;
    msg.state_sequence = 2;
    msg.observed_tick = 2;
    assert(rpm_apply_remote_player_state(&msg));
    scene = fo4_scene_proxy_backend_stub_get(2);
    assert(scene != 0);
    assert(scene->interpolation_pending);
    assert(fpalv_validate_present(2, &result));
    assert(result.presented_position_match);
    assert(result.presented_rotation_match);

    fo4_scene_proxy_backend_stub_step();
    assert(fpalv_validate_present(2, &result));
    assert(result.validation_passed);
    return 0;
}
