#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_observer_bridge.h"
#include "local_player_observer.h"

int main(void) {
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;
    CommonLibF4PlayerHookObserverBridgeStats stats;

    lpo_init(7);
    clf4_phob_install(7);

    memset(&args, 0, sizeof(args));
    args.event.position.x = 42.0f;
    args.event.position.y = 9.0f;
    args.event.rotation.yaw = 11.0f;
    args.event.stance = STANCE_RUN;
    assert(clf4_phob_submit(&args));

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.player_id == 7);
    assert(out.position.x == 42.0f);
    assert(out.position.y == 9.0f);
    assert(out.rotation.yaw == 11.0f);
    assert(out.stance == STANCE_RUN);

    stats = clf4_phob_stats();
    assert(stats.provider_installed);
    assert(stats.has_state);
    assert(stats.submit_count == 1);

    clf4_phob_uninstall();
    return 0;
}
