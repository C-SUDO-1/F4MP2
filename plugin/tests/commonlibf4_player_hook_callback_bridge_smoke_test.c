#include <assert.h>
#include <string.h>
#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "local_player_observer.h"

static bool bridge_cb(const CommonLibF4PlayerHookArgs* args, void* user) {
    (void)user;
    return clf4_phob_submit(args);
}

int main(void) {
    CommonLibF4PlayerHookArgs in;
    CommonLibF4PlayerHookCallbackRegistration reg;
    LocalPlayerObservedState observed;
    memset(&in, 0, sizeof(in));
    memset(&observed, 0, sizeof(observed));

    lpo_init(2);
    clf4_phob_install(2);
    reg.callback = bridge_cb;
    reg.user = 0;
    clf4_player_hook_callback_stub_register(&reg);

    in.event.player_id = 2;
    in.event.position.x = 42.0f;
    in.event.position.y = 5.0f;
    in.event.rotation.yaw = 180.0f;
    in.event.stance = STANCE_RUN;

    assert(clf4_player_hook_callback_stub_emit(&in));
    assert(lpo_capture(&observed));
    assert(observed.player_id == 2);
    assert(observed.position.x == 42.0f);
    assert(observed.rotation.yaw == 180.0f);
    assert(observed.stance == STANCE_RUN);

    clf4_player_hook_callback_stub_clear();
    clf4_phob_uninstall();
    return 0;
}
