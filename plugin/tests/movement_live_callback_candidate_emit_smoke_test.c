#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_live_callback_candidate.h"
#include "commonlibf4_player_hook_callback_stub.h"
#include "movement_sync_bridge.h"

int main(void) {
    CommonLibF4PlayerHookLiveCallbackCandidateConfig cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;

    lpo_init(151);
    msb_init(NULL);
    memset(&cfg, 0, sizeof(cfg));
    cfg.install_config.expected_local_player_id = 151;
    cfg.install_config.require_hook_correct = true;
    cfg.install_config.patch_allowed = true;
    cfg.install_config.require_update_address = true;
    cfg.install_config.require_input_address = true;
    cfg.install_config.require_callback_after_install = true;
    cfg.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.install_config.site_prototype = clf4_phdsp_default();
    cfg.require_supported_runtime_profile = true;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phlcc_activate_with_provider(&f4se, &provider, &cfg));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 151;
    args.event.position.x = 21.0f;
    args.event.position.y = 1.0f;
    args.event.rotation.yaw = 90.0f;
    args.event.velocity.x = 3.0f;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 151);
    assert(msg.position.x == 21.0f);
    assert(msg.rotation.yaw == 90.0f);
    clf4_phlcc_reset();
    return 0;
}
