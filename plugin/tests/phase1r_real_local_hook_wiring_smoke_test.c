#include <assert.h>
#include <string.h>

#include "phase1r_real_local_hook_wiring.h"
#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_proxy_actor_live_validation.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"

static void setup_proxy_side(PlayerId local_player_id, PlayerId remote_player_id) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    MsgPlayerState msg;
    Fo4ProxyActorLiveValidationResult result;

    fpad_reset_default_driver();
    fpasc_reset();
    fpalv_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.local_player_id = local_player_id;
    cfg.default_proxy_base_form_id = 0x6262u;
    cfg.suppress_self_spawn = true;
    cfg.require_base_form_id = true;
    fpalc_configure(&cfg);

    rpm_init(local_player_id);
    fo4_scene_proxy_backend_stub_install();

    memset(&msg, 0, sizeof(msg));
    msg.player_id = remote_player_id;
    msg.position.x = 5.0f;
    msg.position.y = 1.0f;
    msg.rotation.yaw = 45.0f;
    msg.state_sequence = 1;
    msg.observed_tick = 1;
    assert(rpm_apply_remote_player_state(&msg));
    assert(fpalv_validate_present(remote_player_id, &result));
    assert(result.validation_passed);
}

int main(void) {
    Phase1RRealLocalHookWiringConfig cfg;
    Phase1RRealLocalHookWiringState st;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B00BFu, 0, 0 };

    lpo_init(162);
    msb_init(NULL);
    setup_proxy_side(162, 77);

    memset(&cfg, 0, sizeof(cfg));
    cfg.require_wire_ready_after_activation = true;
    cfg.emit_validation_sample = true;
    cfg.build_movement_message = true;
    cfg.candidate_config.install_config.expected_local_player_id = 162;
    cfg.candidate_config.install_config.require_hook_correct = true;
    cfg.candidate_config.install_config.patch_allowed = true;
    cfg.candidate_config.install_config.require_update_address = true;
    cfg.candidate_config.install_config.require_input_address = true;
    cfg.candidate_config.install_config.require_callback_after_install = true;
    cfg.candidate_config.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.candidate_config.install_config.site_prototype = clf4_phdsp_default();
    cfg.candidate_config.callback_source_symbol = "PlayerCharacter::Update";
    cfg.candidate_config.callback_guard_symbol = "PlayerControls::Update";
    cfg.candidate_config.require_supported_runtime_profile = true;
    cfg.callback_validation_config.expected_local_player_id = 162;
    cfg.callback_validation_config.min_callback_forward_count = 1;
    cfg.callback_validation_config.min_callback_accept_count = 1;
    cfg.callback_validation_config.require_runtime_profile_supported = true;
    cfg.callback_validation_config.require_callback_registered = true;
    cfg.callback_validation_config.require_provider_ready = true;
    cfg.callback_validation_config.require_captured_state = true;
    cfg.callback_validation_config.require_expected_player_id_match = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.local_player_id = 162;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.remote_player_id = 77;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_toolchain_match = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_callback_validation = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_proxy_validation = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_remote_runtime_present = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_scene_present = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_driver_present = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.callback_validation_config = cfg.callback_validation_config;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.require_candidate_activated = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.require_runtime_profile_supported = true;
    cfg.readiness_config.wire_plan_config.fallback_config.arming_config.require_patch_policy_armed = true;
    cfg.validation_sample.event.player_id = 162;
    cfg.validation_sample.event.position.x = 16.2f;
    cfg.validation_sample.event.position.y = 1.0f;
    cfg.validation_sample.event.rotation.yaw = 30.0f;
    cfg.validation_sample.event.velocity.x = 3.0f;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(phase1r_rlhw_attempt(&f4se, &provider, &cfg));
    st = phase1r_rlhw_state();
    assert(st.wired);
    assert(st.activated);
    assert(st.emitted_validation_sample);
    assert(st.callback_validation_passed);
    assert(st.wire_ready_after_activation);
    assert(st.movement_message_built);
    assert(st.emitted_message.player_id == 162);
    assert(st.emitted_message.position.x == 16.2f);
    assert(st.recommended_action == PHASE1R_RHWP_ACTION_WIRE_REAL_HOOK);
    return 0;
}
