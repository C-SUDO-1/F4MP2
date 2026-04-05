#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "commonlibf4_player_hook_live_callback_validation.h"
#include "fo4_proxy_actor_driver.h"
#include "fo4_proxy_actor_spawn_contract.h"
#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "movement_sync_bridge.h"
#include "phase1r_in_engine_goal0_validation.h"
#include "proxy_runtime.h"
#include "remote_proxy_manager.h"

int main(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig callback_cfg;
    Fo4ProxyActorLiveCandidateConfig proxy_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B00BFu, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;
    Phase1RInEngineGoal0ValidationConfig harness_cfg;
    Phase1RInEngineGoal0ValidationState state;

    proxy_runtime_init();
    fpad_reset_default_driver();
    fpasc_reset();
    fpalc_reset();
    clf4_phlcc_reset();
    phase1r_igmv_reset();

    lpo_init(301);
    msb_init(NULL);

    memset(&callback_cfg, 0, sizeof(callback_cfg));
    callback_cfg.install_config.expected_local_player_id = 301;
    callback_cfg.install_config.require_hook_correct = true;
    callback_cfg.install_config.patch_allowed = true;
    callback_cfg.install_config.require_update_address = true;
    callback_cfg.install_config.require_input_address = true;
    callback_cfg.install_config.require_callback_after_install = true;
    callback_cfg.install_config.patch_policy = clf4_phpp_default_armed();
    callback_cfg.install_config.site_prototype = clf4_phdsp_default();
    callback_cfg.callback_source_symbol = "PlayerCharacter::Update";
    callback_cfg.callback_guard_symbol = "PlayerControls::Update";
    callback_cfg.require_supported_runtime_profile = true;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phlcc_activate_with_provider(&f4se, &provider, &callback_cfg));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 301;
    args.event.position.x = 11.0f;
    args.event.position.y = 22.0f;
    args.event.position.z = 3.0f;
    args.event.velocity.x = 1.25f;
    args.event.rotation.yaw = 45.0f;
    args.event.stance = STANCE_RUN;
    args.event.anim_state = 9u;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 301);

    memset(&proxy_cfg, 0, sizeof(proxy_cfg));
    proxy_cfg.local_player_id = 1;
    proxy_cfg.default_proxy_base_form_id = 0x9911u;
    proxy_cfg.suppress_self_spawn = true;
    proxy_cfg.require_base_form_id = true;
    fpalc_configure(&proxy_cfg);

    rpm_init(1);
    fo4_scene_proxy_backend_stub_install();
    assert(rpm_apply_remote_player_state(&msg));

    memset(&harness_cfg, 0, sizeof(harness_cfg));
    harness_cfg.local_player_id = 301;
    harness_cfg.remote_player_id = 301;
    harness_cfg.require_toolchain_match = true;
    harness_cfg.require_callback_validation = true;
    harness_cfg.require_proxy_validation = true;
    harness_cfg.require_remote_runtime_present = true;
    harness_cfg.require_scene_present = true;
    harness_cfg.require_driver_present = true;
    harness_cfg.callback_validation_config.expected_local_player_id = 301;
    harness_cfg.callback_validation_config.min_callback_forward_count = 1;
    harness_cfg.callback_validation_config.min_callback_accept_count = 1;
    harness_cfg.callback_validation_config.require_runtime_profile_supported = true;
    harness_cfg.callback_validation_config.require_callback_registered = true;
    harness_cfg.callback_validation_config.require_provider_ready = true;
    harness_cfg.callback_validation_config.require_captured_state = true;
    harness_cfg.callback_validation_config.require_expected_player_id_match = true;

    assert(phase1r_igmv_validate_current(&harness_cfg));
    state = phase1r_igmv_state();
    assert(state.validated);
    assert(state.toolchain_manifest_match);
    assert(state.callback_validation_passed);
    assert(state.proxy_validation_passed);
    assert(state.remote_runtime_present);
    assert(state.scene_present);
    assert(state.driver_present);
    assert(state.callback_state.observed_player_id == 301);
    assert(state.proxy_state.player_id == 301);
    assert(state.proxy_state.validation_passed);
    assert(state.proxy_state.proxy_base_form_id == 0x9911u);
    return 0;
}
