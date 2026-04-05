#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_install_candidate.h"
#include "movement_sync_bridge.h"

int main(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    CommonLibF4PlayerHookInstallCandidateStats stats;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    MsgPlayerState msg;

    lpo_init(43);
    msb_init(NULL);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 43;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phic_install_with_provider(&ok, &provider, &cfg));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 43;
    args.event.position.x = 11.0f;
    args.event.position.y = 4.0f;
    args.event.position.z = 1.0f;
    args.event.rotation.yaw = 75.0f;
    args.event.velocity.x = 3.5f;
    args.event.velocity.y = 0.25f;
    args.event.stance = STANCE_WALK;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 43);
    assert(msg.position.x == 11.0f);
    assert(msg.position.y == 4.0f);
    assert(msg.position.z == 1.0f);
    assert(msg.rotation.yaw == 75.0f);
    assert(msg.velocity.x == 3.5f);
    assert(msg.velocity.y == 0.25f);
    assert(msg.stance == STANCE_WALK);

    stats = clf4_phic_stats();
    assert(stats.path_ready);
    assert(stats.callback_accept_count == 1);
    assert(stats.last_state.position.x == 11.0f);
    assert(stats.last_state.rotation.yaw == 75.0f);

    clf4_phic_uninstall();
    return 0;
}
