#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "commonlibf4_player_hook_callback_stub.h"
#include "local_player_observer.h"

int main(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    CommonLibF4PlayerHookLiveCallbackCandidateState st;
    LocalPlayerObservedState observed;

    memset(&cfg, 0, sizeof(cfg));
    cfg.install_config.expected_local_player_id = 151;
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
    lpo_init(151);
    assert(clf4_phlcc_activate_with_provider(&f4se, &provider, &cfg));
    st = clf4_phlcc_state();
    assert(st.activated);
    assert(st.callback_registered);
    assert(st.provider_ready);
    assert(st.runtime_profile_supported);
    assert(strcmp(st.callback_source_symbol, "PlayerCharacter::Update") == 0);
    assert(strcmp(st.callback_guard_symbol, "PlayerControls::Update") == 0);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 151;
    args.event.position.x = 15.0f;
    args.event.rotation.yaw = 45.0f;
    assert(clf4_player_hook_callback_stub_emit(&args));
    st = clf4_phlcc_state();
    assert(st.callback_accept_count >= 1);
    assert(lpo_capture(&observed));
    assert(observed.player_id == 151);
    assert(observed.position.x == 15.0f);
    clf4_phlcc_reset();
    return 0;
}
