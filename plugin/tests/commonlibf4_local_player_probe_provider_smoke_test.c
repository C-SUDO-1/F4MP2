#include <assert.h>
#include <string.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_local_player_probe_provider.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    CommonLibF4LocalPlayerProbeProviderConfig provider_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;
    CommonLibF4LocalPlayerProbeProviderStats stats;

    lpo_init(17);
    clf4_phrp_reset();
    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = 17;
    probe_cfg.require_update_address = true;
    probe_cfg.require_input_address = true;
    probe_cfg.require_callback_after_install = true;
    clf4_phrp_configure(&probe_cfg);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));

    memset(&provider_cfg, 0, sizeof(provider_cfg));
    provider_cfg.expected_local_player_id = 17;
    provider_cfg.require_hook_correct = true;
    clf4_lppp_install(&provider_cfg);
    assert(clf4_lppp_installed());

    memset(&out, 0, sizeof(out));
    assert(!lpo_capture(&out));

    memset(&args, 0, sizeof(args));
    args.event.player_id = 17;
    args.event.position.x = 12.0f;
    args.event.position.y = 4.0f;
    args.event.rotation.yaw = 55.0f;
    args.event.velocity.x = 3.0f;
    assert(clf4_phrp_submit_callback(&args));

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.player_id == 17);
    assert(out.position.x == 12.0f);
    assert(out.position.y == 4.0f);
    assert(out.rotation.yaw == 55.0f);

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    stats = clf4_lppp_stats();
    assert(stats.capture_count == 2);
    assert(stats.stale_capture_count == 1);
    assert(stats.blocked_capture_count == 1);
    assert(stats.last_callback_count == 1);
    assert(stats.last_state.position.x == 12.0f);

    clf4_lppp_uninstall();
    assert(!clf4_lppp_installed());
    return 0;
}
