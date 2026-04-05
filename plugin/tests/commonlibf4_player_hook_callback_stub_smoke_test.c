#include <assert.h>
#include <string.h>
#include "commonlibf4_player_hook_callback_stub.h"

static bool test_cb(const CommonLibF4PlayerHookArgs* args, void* user) {
    CommonLibF4PlayerHookArgs* out = (CommonLibF4PlayerHookArgs*)user;
    *out = *args;
    return true;
}

int main(void) {
    CommonLibF4PlayerHookArgs captured;
    CommonLibF4PlayerHookArgs in;
    CommonLibF4PlayerHookCallbackRegistration reg;
    CommonLibF4PlayerHookCallbackStubStats stats;
    memset(&captured, 0, sizeof(captured));
    memset(&in, 0, sizeof(in));
    in.event.player_id = 7;
    in.event.position.x = 10.0f;
    in.event.rotation.yaw = 90.0f;

    reg.callback = test_cb;
    reg.user = &captured;
    clf4_player_hook_callback_stub_register(&reg);
    assert(clf4_player_hook_callback_stub_emit(&in));
    stats = clf4_player_hook_callback_stub_stats();
    assert(stats.registered_callback);
    assert(stats.emit_count == 1);
    assert(stats.callback_success_count == 1);
    assert(stats.has_last_args);
    assert(captured.event.player_id == 7);
    assert(captured.event.position.x == 10.0f);
    assert(captured.event.rotation.yaw == 90.0f);
    clf4_player_hook_callback_stub_clear();
    return 0;
}
