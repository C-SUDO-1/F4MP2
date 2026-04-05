#include <assert.h>
#include <string.h>
#include "commonlibf4_player_hook_observer_bridge.h"
#include "local_player_observer.h"

int main(void) {
    CommonLibF4PlayerHookArgs args;
    LocalPlayerObservedState out;
    CommonLibF4PlayerHookObserverBridgeStats st;
    memset(&args, 0, sizeof(args));
    clf4_phob_install(77);

    args.event.player_id = 77;
    args.event.position.x = 1.0f;
    assert(clf4_phob_submit(&args));
    args.event.position.x = 2.0f;
    assert(clf4_phob_submit(&args));
    args.event.position.x = 3.0f;
    assert(clf4_phob_submit(&args));

    memset(&out, 0, sizeof(out));
    assert(lpo_capture_state(&out));
    assert(out.valid);
    assert(out.position.x == 3.0f);

    st = clf4_phob_stats();
    assert(st.submit_count == 3);
    assert(st.capture_count >= 1);
    assert(st.queue_high_watermark >= 3);
    assert(st.last_state.position.x == 3.0f);

    clf4_phob_uninstall();
    return 0;
}
