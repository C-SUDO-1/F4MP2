#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "commonlibf4_player_hook_live_callback_validation.h"
#include "commonlibf4_player_hook_callback_stub.h"

int main(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig cfg;
    CommonLibF4PlayerHookLiveCallbackValidationConfig vcfg;
    CommonLibF4PlayerHookLiveCallbackValidationState vst;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;

    memset(&cfg, 0, sizeof(cfg));
    cfg.install_config.expected_local_player_id = 153;
    cfg.install_config.require_hook_correct = true;
    cfg.install_config.patch_allowed = true;
    cfg.install_config.require_update_address = true;
    cfg.install_config.require_input_address = true;
    cfg.install_config.require_callback_after_install = true;
    cfg.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.install_config.site_prototype = clf4_phdsp_default();
    cfg.callback_source_symbol = "PlayerCharacter::Update";
    cfg.callback_guard_symbol = "PlayerControls::Update";
    cfg.require_supported_runtime_profile = true;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    lpo_init(153);
    assert(clf4_phlcc_activate_with_provider(&f4se, &provider, &cfg));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 153;
    args.event.position.x = 30.0f;
    args.event.rotation.yaw = 135.0f;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&vcfg, 0, sizeof(vcfg));
    vcfg.expected_local_player_id = 153;
    vcfg.min_callback_forward_count = 1;
    vcfg.min_callback_accept_count = 1;
    vcfg.require_runtime_profile_supported = true;
    vcfg.require_callback_registered = true;
    vcfg.require_provider_ready = true;
    vcfg.require_captured_state = true;
    vcfg.require_expected_player_id_match = true;

    assert(clf4_phlcv_validate_current(&vcfg));
    vst = clf4_phlcv_state();
    assert(vst.validated);
    assert(vst.failure == CLF4_PHLV_FAIL_NONE);
    assert(vst.callback_accept_count >= 1);
    assert(vst.observed_player_id == 153);
    assert(strcmp(vst.site_prototype_name, "player_character_update_before_input") == 0);
    assert(strcmp(vst.runtime_profile_name, "fo4_1_10_984_player_character_update_before_input") == 0);
    assert(strcmp(vst.callback_source_symbol, "PlayerCharacter::Update") == 0);
    clf4_phlcc_reset();
    return 0;
}
