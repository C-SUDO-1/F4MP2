#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_proxy_actor_live_validation.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"

int main(void) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    MsgPlayerState msg;
    Fo4ProxyActorLiveValidationResult result;

    fpad_reset_default_driver();
    fpasc_reset();
    fpalv_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = 1;
    cfg.default_proxy_base_form_id = 0x7373u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    cfg.actor_class_name = "Actor";
    cfg.create_api_symbol = "TESObjectREFR::Create";
    cfg.move_api_symbol = "TESObjectREFR::SetPosition";
    cfg.rotate_api_symbol = "TESObjectREFR::SetRotation";
    cfg.despawn_api_symbol = "TESObjectREFR::Disable";
    fpalc_configure(&cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 5.0f;
    msg.position.y = 7.0f;
    msg.rotation.yaw = 15.0f;
    msg.velocity.x = 2.0f;
    msg.stance = 1;
    msg.anim_state = 12;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));
    assert(fpalv_validate_present(2, &result));
    assert(result.validation_passed);
    assert(result.scene_present);
    assert(result.driver_present);
    assert(result.actor_handle_match);
    assert(result.presented_position_match);
    assert(result.presented_rotation_match);
    assert(result.proxy_base_form_id_present);
    assert(result.spawn_seen);
    assert(result.move_seen);
    assert(result.rotate_seen);

    assert(rpm_remove_remote_player(2));
    assert(fpalv_validate_despawn(2, &result));
    assert(result.validation_passed);
    assert(result.scene_despawned);
    assert(result.driver_despawned);
    assert(result.despawn_seen);
    return 0;
}
