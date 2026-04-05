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

    lpo_init(88);
    msb_init(NULL);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 88;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;
    cfg.patch_policy = clf4_phpp_default_dry_run();
    cfg.site_prototype = clf4_phdsp_default();

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(!clf4_phic_install_with_provider(&ok, &provider, &cfg));
    memset(&msg, 0, sizeof(msg));
    assert(!msb_build_player_state_if_changed(&msg));

    cfg.patch_policy = clf4_phpp_default_armed();
    assert(clf4_phic_install_with_provider(&ok, &provider, &cfg));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 88;
    args.event.position.x = 8.0f;
    args.event.position.y = 6.0f;
    args.event.rotation.yaw = 30.0f;
    args.event.velocity.x = 1.25f;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&msg, 0, sizeof(msg));
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.player_id == 88);
    assert(msg.position.x == 8.0f);
    assert(msg.rotation.yaw == 30.0f);

    stats = clf4_phic_stats();
    assert(stats.armed_install);
    assert(stats.patch_step_attempted);
    assert(stats.callback_accept_count == 1);
    clf4_phic_uninstall();
    return 0;
}
