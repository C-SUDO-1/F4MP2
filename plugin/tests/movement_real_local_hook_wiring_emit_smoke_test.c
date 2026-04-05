#include <assert.h>
#include <string.h>

#include "phase1r_real_local_hook_wiring.h"

int main(void) {
    Phase1RRealLocalHookWiringConfig cfg;
    Phase1RRealLocalHookWiringState st;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B00BFu, 0, 0 };

    lpo_init(2162);
    msb_init(NULL);

    memset(&cfg, 0, sizeof(cfg));
    cfg.require_wire_ready_after_activation = false;
    cfg.emit_validation_sample = true;
    cfg.build_movement_message = true;
    cfg.candidate_config.install_config.expected_local_player_id = 2162;
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
    cfg.callback_validation_config.expected_local_player_id = 2162;
    cfg.callback_validation_config.min_callback_forward_count = 1;
    cfg.callback_validation_config.min_callback_accept_count = 1;
    cfg.callback_validation_config.require_runtime_profile_supported = true;
    cfg.callback_validation_config.require_callback_registered = true;
    cfg.callback_validation_config.require_provider_ready = true;
    cfg.callback_validation_config.require_captured_state = true;
    cfg.callback_validation_config.require_expected_player_id_match = true;
    cfg.validation_sample.event.player_id = 2162;
    cfg.validation_sample.event.position.x = 21.62f;
    cfg.validation_sample.event.rotation.yaw = 90.0f;
    cfg.validation_sample.event.velocity.x = 6.0f;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(phase1r_rlhw_attempt(&f4se, &provider, &cfg));
    st = phase1r_rlhw_state();
    assert(st.movement_message_built);
    assert(st.emitted_message.player_id == 2162);
    assert(st.emitted_message.position.x == 21.62f);
    assert(st.emitted_message.rotation.yaw == 90.0f);
    return 0;
}
