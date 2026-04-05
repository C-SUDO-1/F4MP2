#include <assert.h>
#include <string.h>

#include "local_player_observer.h"
#include "local_player_observer_provider.h"

int main(void) {
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;
    LocalPlayerObserverLiveProviderStats stats;

    memset(&args, 0, sizeof(args));
    memset(&out, 0, sizeof(out));

    lpo_init(7);
    lpo_live_provider_install(7);

    args.event.position.x = 99.0f;
    args.event.position.y = 5.0f;
    args.event.rotation.yaw = 123.0f;
    args.event.stance = STANCE_CROUCH;

    assert(lpo_live_provider_submit(&args));
    assert(lpo_capture(&out));
    assert(out.valid);
    assert(out.player_id == 7);
    assert(out.position.x == 99.0f);
    assert(out.rotation.yaw == 123.0f);
    assert(out.stance == STANCE_CROUCH);

    stats = lpo_live_provider_stats();
    assert(stats.installed);
    assert(stats.has_state);
    assert(stats.submit_count == 1);
    assert(stats.capture_count >= 1);
    assert(stats.live_accept_count == 1);
    assert(stats.live_reject_count == 0);
    assert(stats.last_used_default_player_id);

    args.event.stance = 255;
    assert(!lpo_live_provider_submit(&args));
    stats = lpo_live_provider_stats();
    assert(stats.live_reject_count == 1);
    assert(stats.last_reject_reason == CLF4_PLA_REJECT_INVALID_STANCE);

    lpo_live_provider_uninstall();
    return 0;
}
