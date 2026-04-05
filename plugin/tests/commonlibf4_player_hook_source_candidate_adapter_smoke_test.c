#include <assert.h>
#include <string.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_player_hook_source_candidate_adapter.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProbeConfig probe_cfg;
    CommonLibF4PlayerHookSourceCandidateAdapterConfig adapter_cfg;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;
    CommonLibF4PlayerHookSourceCandidateAdapterStats stats;
    CommonLibF4PlayerHookCallbackStubStats callback_stats;

    lpo_init(31);
    clf4_phrp_reset();

    memset(&probe_cfg, 0, sizeof(probe_cfg));
    probe_cfg.expected_local_player_id = 31;
    probe_cfg.require_update_address = true;
    probe_cfg.require_input_address = true;
    probe_cfg.require_callback_after_install = true;
    clf4_phrp_configure(&probe_cfg);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));

    memset(&adapter_cfg, 0, sizeof(adapter_cfg));
    adapter_cfg.expected_local_player_id = 31;
    adapter_cfg.require_hook_correct = true;
    clf4_phsca_install(&adapter_cfg);
    assert(clf4_phsca_installed());

    memset(&args, 0, sizeof(args));
    args.event.player_id = 31;
    args.event.position.x = 14.0f;
    args.event.position.y = -6.0f;
    args.event.rotation.yaw = 87.0f;
    args.event.velocity.x = 1.5f;
    args.event.stance = STANCE_RUN;
    assert(clf4_player_hook_callback_stub_emit(&args));

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.player_id == 31);
    assert(out.position.x == 14.0f);
    assert(out.rotation.yaw == 87.0f);

    stats = clf4_phsca_stats();
    assert(stats.installed);
    assert(stats.callback_registered);
    assert(stats.provider_installed);
    assert(stats.callback_forward_count == 1);
    assert(stats.callback_accept_count == 1);
    assert(stats.callback_reject_count == 0);
    assert(stats.last_probe_callback_count == 1);
    assert(stats.has_state);
    assert(stats.last_state.player_id == 31);

    callback_stats = clf4_player_hook_callback_stub_stats();
    assert(callback_stats.registered_callback);
    assert(callback_stats.emit_count == 1);
    assert(callback_stats.callback_success_count == 1);

    clf4_phsca_uninstall();
    assert(!clf4_phsca_installed());
    return 0;
}
