#include <assert.h>
#include <string.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_player_hook_source_candidate_adapter.h"
#include "movement_sync_bridge.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    CommonLibF4PlayerHookSourceCandidateAdapterConfig adapter_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;
    CommonLibF4PlayerHookSourceCandidateAdapterStats stats;

    lpo_init(37);
    msb_init(NULL);
    clf4_phrp_reset();

    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = 37;
    probe_cfg.require_update_address = true;
    probe_cfg.require_input_address = true;
    probe_cfg.require_callback_after_install = true;
    clf4_phrp_configure(&probe_cfg);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));

    memset(&adapter_cfg, 0, sizeof(adapter_cfg));
    adapter_cfg.expected_local_player_id = 37;
    adapter_cfg.require_hook_correct = true;
    clf4_phsca_install(&adapter_cfg);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 37;
    args.event.position.x = 3.0f;
    args.event.position.y = 9.0f;
    args.event.rotation.yaw = 120.0f;
    args.event.velocity.x = 4.5f;
    args.event.stance = STANCE_WALK;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 37);
    assert(msg.position.x == 3.0f);
    assert(msg.position.y == 9.0f);
    assert(msg.rotation.yaw == 120.0f);
    assert(msg.velocity.x == 4.5f);
    assert(msg.stance == STANCE_WALK);

    stats = clf4_phsca_stats();
    assert(stats.callback_accept_count == 1);
    assert(stats.last_state.position.x == 3.0f);

    clf4_phsca_uninstall();
    return 0;
}
