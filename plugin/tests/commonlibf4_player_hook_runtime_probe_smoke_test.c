#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_runtime_probe.h"
#include "commonlibf4_player_hook_patch_policy.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProbeConfig cfg;
    CommonLibF4PlayerHookRuntimeProbeState st;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;

    clf4_phrp_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 11;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;
    clf4_phrp_configure(&cfg);

    provider = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
    assert(!clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));
    st = clf4_phrp_state();
    assert(st.install_attempted);
    assert(!st.install_ready);
    assert(st.install_state == HOOK_INSTALL_PARTIAL);
    assert(st.install_error == HOOK_INSTALL_ERR_ADDRESS_MISSING);
    assert(hook_install_registry_get_state(HOOK_FAMILY_PLAYER) == HOOK_INSTALL_PARTIAL);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phrp_install_with_provider(&ok, &provider, true, clf4_phpp_default_armed()));
    st = clf4_phrp_state();
    assert(st.install_ready);
    assert(st.resolved_update_addr != 0);
    assert(st.resolved_input_addr != 0);
    assert(!st.callback_ready);
    assert(!st.hook_correct);
    assert(hook_install_registry_is_installed(HOOK_FAMILY_PLAYER));

    memset(&args, 0, sizeof(args));
    args.event.position.x = 1.0f;
    args.event.rotation.yaw = 90.0f;
    args.event.velocity.x = 2.0f;
    args.event.player_id = 11;
    assert(clf4_phrp_submit_callback(&args));
    st = clf4_phrp_state();
    assert(st.callback_count == 1);
    assert(st.last_seen_player_id == 11);
    assert(st.callback_ready);
    assert(st.hook_correct);
    assert(st.last_sample.state.position.x == 1.0f);

    memset(&args, 0, sizeof(args));
    args.event.position.x = 2.0f;
    args.event.rotation.yaw = 45.0f;
    args.event.velocity.x = 3.0f;
    args.event.player_id = 99;
    assert(!clf4_phrp_submit_callback(&args));
    st = clf4_phrp_state();
    assert(st.callback_count == 2);
    assert(st.mismatched_player_count == 1);
    assert(!st.hook_correct);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 11;
    args.event.position.x = 5.0f;
    args.event.rotation.yaw = 15.0f;
    args.event.velocity.x = 0.0f / 0.0f;
    assert(!clf4_phrp_submit_callback(&args));
    st = clf4_phrp_state();
    assert(st.rejected_callback_count == 1);
    assert(st.last_reject_reason == CLF4_PLA_REJECT_INVALID_VELOCITY);
    return 0;
}
