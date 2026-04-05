#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_install_candidate.h"

int main(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    CommonLibF4PlayerHookInstallCandidateStats stats;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;

    lpo_init(41);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 41;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;

    provider = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
    assert(!clf4_phic_install_with_provider(&ok, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.configured);
    assert(stats.install_attempted);
    assert(!stats.install_ready);
    assert(!stats.adapter_installed);
    assert(!stats.callback_registered);
    assert(stats.install_state == HOOK_INSTALL_PARTIAL);
    assert(stats.install_error == HOOK_INSTALL_ERR_ADDRESS_MISSING);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phic_install_with_provider(&ok, &provider, &cfg));
    assert(clf4_phic_installed());

    stats = clf4_phic_stats();
    assert(stats.install_ready);
    assert(stats.adapter_installed);
    assert(stats.callback_registered);
    assert(stats.provider_installed);
    assert(stats.path_ready);
    assert(stats.resolved_update_addr != 0);
    assert(stats.resolved_input_addr != 0);
    assert(!stats.hook_correct);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 41;
    args.event.position.x = 7.0f;
    args.event.position.y = -2.0f;
    args.event.rotation.yaw = 135.0f;
    args.event.velocity.x = 1.25f;
    args.event.stance = STANCE_RUN;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.player_id == 41);
    assert(out.position.x == 7.0f);
    assert(out.rotation.yaw == 135.0f);

    stats = clf4_phic_stats();
    assert(stats.hook_correct);
    assert(stats.callback_forward_count == 1);
    assert(stats.callback_accept_count == 1);
    assert(stats.callback_reject_count == 0);
    assert(stats.last_probe_callback_count == 1);
    assert(stats.has_state);
    assert(stats.last_state.player_id == 41);

    clf4_phic_uninstall();
    assert(!clf4_phic_installed());
    return 0;
}
