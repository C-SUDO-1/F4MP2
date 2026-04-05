#include <assert.h>
#include <string.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_local_player_probe_provider.h"
#include "movement_sync_bridge.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    CommonLibF4LocalPlayerProbeProviderConfig provider_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;

    lpo_init(23);
    msb_init(NULL);
    clf4_phrp_reset();

    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = 23;
    probe_cfg.require_update_address = true;
    probe_cfg.require_input_address = true;
    probe_cfg.require_callback_after_install = true;
    clf4_phrp_configure(&probe_cfg);
    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));

    memset(&provider_cfg, 0, sizeof(provider_cfg));
    provider_cfg.expected_local_player_id = 23;
    provider_cfg.require_hook_correct = true;
    clf4_lppp_install(&provider_cfg);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 23;
    args.event.position.x = 7.0f;
    args.event.position.y = -3.0f;
    args.event.rotation.yaw = 66.0f;
    args.event.velocity.x = 2.5f;
    args.event.stance = STANCE_RUN;
    assert(clf4_phrp_submit_callback(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 23);
    assert(msg.position.x == 7.0f);
    assert(msg.position.y == -3.0f);
    assert(msg.rotation.yaw == 66.0f);
    assert(msg.velocity.x == 2.5f);
    assert(msg.stance == STANCE_RUN);
    assert(clf4_lppp_stats().capture_count >= 1);

    clf4_lppp_uninstall();
    return 0;
}
