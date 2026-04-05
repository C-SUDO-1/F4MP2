#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "commonlibf4_player_live_adapter.h"
#include "local_player_observer.h"

static bool bridge_cb(const CommonLibF4PlayerHookArgs* args, void* user) {
    (void)user;
    return clf4_phob_submit(args);
}

int main(void) {
    CommonLibF4PlayerHookArgs args;
    CommonLibF4PlayerHookCallbackRegistration reg;
    CommonLibF4PlayerHookObserverBridgeStats bridge_stats;
    CommonLibF4PlayerLiveAdapterStats adapter_stats;
    LocalPlayerObservedState observed;

    memset(&args, 0, sizeof(args));
    memset(&observed, 0, sizeof(observed));

    lpo_init(21);
    clf4_phob_install(21);
    reg.callback = bridge_cb;
    reg.user = 0;
    clf4_player_hook_callback_stub_register(&reg);

    args.event.position.x = 12.0f;
    args.event.rotation.yaw = 33.0f;
    args.event.stance = STANCE_WALK;
    assert(clf4_player_hook_callback_stub_emit(&args));

    assert(lpo_capture(&observed));
    assert(observed.valid);
    assert(observed.player_id == 21);
    assert(observed.position.x == 12.0f);
    assert(observed.rotation.yaw == 33.0f);

    bridge_stats = clf4_phob_stats();
    assert(bridge_stats.submit_count == 1);
    assert(bridge_stats.live_accept_count == 1);
    assert(bridge_stats.live_reject_count == 0);
    assert(bridge_stats.last_observed_tick == 1);
    assert(bridge_stats.last_source_kind == CLF4_PLA_SOURCE_HOOK_CALLBACK);
    assert(bridge_stats.last_used_default_player_id);

    adapter_stats = clf4_pla_stats();
    assert(adapter_stats.accept_count == 1);
    assert(adapter_stats.reject_count == 0);
    assert(adapter_stats.last_sample.state.player_id == 21);

    args.event.stance = 255;
    assert(!clf4_player_hook_callback_stub_emit(&args));
    bridge_stats = clf4_phob_stats();
    assert(bridge_stats.submit_count == 1);
    assert(bridge_stats.live_reject_count == 1);

    adapter_stats = clf4_pla_stats();
    assert(adapter_stats.reject_count == 1);
    assert(adapter_stats.last_reject_reason == CLF4_PLA_REJECT_INVALID_STANCE);

    clf4_player_hook_callback_stub_clear();
    clf4_phob_uninstall();
    return 0;
}
