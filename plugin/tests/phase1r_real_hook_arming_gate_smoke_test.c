#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "fo4_proxy_actor_driver.h"
#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_proxy_actor_spawn_contract.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "movement_sync_bridge.h"
#include "phase1r_real_hook_arming_gate.h"
#include "proxy_runtime.h"
#include "remote_proxy_manager.h"

int main(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig callback_cfg;
    Fo4ProxyActorLiveCandidateConfig proxy_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B00BFu, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;
    Phase1RRealHookArmingGateConfig cfg;
    Phase1RRealHookArmingGateState state;

    proxy_runtime_init(); fpad_reset_default_driver(); fpasc_reset(); fpalc_reset(); clf4_phlcc_reset(); phase1r_rhag_reset();
    lpo_init(402); msb_init(NULL);

    memset(&callback_cfg, 0, sizeof(callback_cfg));
    callback_cfg.install_config.expected_local_player_id = 402;
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
    memset(&args, 0, sizeof(args)); args.event.player_id = 402; args.event.position.x = 21.0f; args.event.rotation.yaw = 90.0f; args.event.stance = STANCE_RUN; assert(clf4_player_hook_callback_stub_emit(&args));
    memset(&msg, 0, sizeof(msg)); assert(msb_build_player_state_if_changed(&msg));
    memset(&proxy_cfg, 0, sizeof(proxy_cfg)); proxy_cfg.local_player_id = 1; proxy_cfg.default_proxy_base_form_id = 0xA158u; proxy_cfg.suppress_self_spawn = true; proxy_cfg.require_base_form_id = true; fpalc_configure(&proxy_cfg); rpm_init(1); fo4_scene_proxy_backend_stub_install(); assert(rpm_apply_remote_player_state(&msg));
    memset(&cfg, 0, sizeof(cfg));
    cfg.evidence_config.validation_config.local_player_id = 402; cfg.evidence_config.validation_config.remote_player_id = 402; cfg.evidence_config.validation_config.require_toolchain_match = true; cfg.evidence_config.validation_config.require_callback_validation = true; cfg.evidence_config.validation_config.require_proxy_validation = true; cfg.evidence_config.validation_config.require_remote_runtime_present = true; cfg.evidence_config.validation_config.require_scene_present = true; cfg.evidence_config.validation_config.require_driver_present = true; cfg.evidence_config.validation_config.callback_validation_config.expected_local_player_id = 402; cfg.evidence_config.validation_config.callback_validation_config.min_callback_forward_count = 1; cfg.evidence_config.validation_config.callback_validation_config.min_callback_accept_count = 1; cfg.evidence_config.validation_config.callback_validation_config.require_runtime_profile_supported = true; cfg.evidence_config.validation_config.callback_validation_config.require_callback_registered = true; cfg.evidence_config.validation_config.callback_validation_config.require_provider_ready = true; cfg.evidence_config.validation_config.callback_validation_config.require_captured_state = true; cfg.evidence_config.validation_config.callback_validation_config.require_expected_player_id_match = true; cfg.require_candidate_activated = true; cfg.require_runtime_profile_supported = true; cfg.require_patch_policy_armed = true;
    assert(phase1r_rhag_evaluate(&cfg)); state = phase1r_rhag_state(); assert(state.arming_allowed); assert(state.failure == PHASE1R_RHAG_FAIL_NONE); assert(state.evidence_ready); assert(state.candidate_activated); assert(state.runtime_profile_supported); assert(state.patch_policy_armed); assert(strcmp(state.site_prototype_name, "player_character_update_before_input") == 0); return 0;
}
