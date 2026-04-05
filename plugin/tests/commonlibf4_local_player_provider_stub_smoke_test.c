#include <assert.h>
#include <string.h>

#include "commonlibf4_local_player_provider_stub.h"
#include "local_player_observer.h"

int main(void) {
    LocalPlayerObservedState in, out;

    lpo_init(7);
    clf4_lpo_stub_install(7);

    memset(&in, 0, sizeof(in));
    in.position.x = 11.0f;
    in.position.y = 22.0f;
    in.rotation.yaw = 33.0f;
    in.stance = STANCE_RUN;
    clf4_lpo_stub_feed(&in);

    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.player_id == 7);
    assert(out.position.x == 11.0f);
    assert(out.position.y == 22.0f);
    assert(out.rotation.yaw == 33.0f);

    clf4_lpo_stub_set_available(false);
    memset(&out, 0, sizeof(out));
    assert(!lpo_capture(&out));

    clf4_lpo_stub_uninstall();
    return 0;
}
