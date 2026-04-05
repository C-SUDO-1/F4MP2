#include <assert.h>
#include <string.h>
#include "commonlibf4_actor_hook_callback_stub.h"

static bool test_cb(const CommonLibF4ActorHookArgs* args, void* user) {
    CommonLibF4ActorHookArgs* out = (CommonLibF4ActorHookArgs*)user;
    *out = *args;
    return true;
}

int main(void) {
    CommonLibF4ActorHookArgs captured;
    CommonLibF4ActorHookArgs in;
    CommonLibF4ActorHookCallbackRegistration reg;
    CommonLibF4ActorHookCallbackStubStats stats;
    memset(&captured, 0, sizeof(captured));
    memset(&in, 0, sizeof(in));
    in.event.actor_net_id = 77;
    in.event.position.x = 1.0f;
    in.event.rotation.yaw = 45.0f;

    reg.callback = test_cb;
    reg.user = &captured;
    clf4_actor_hook_callback_stub_register(&reg);
    assert(clf4_actor_hook_callback_stub_emit(&in));
    stats = clf4_actor_hook_callback_stub_stats();
    assert(stats.registered_callback);
    assert(stats.emit_count == 1);
    assert(stats.callback_success_count == 1);
    assert(stats.has_last_args);
    assert(captured.event.actor_net_id == 77);
    assert(captured.event.position.x == 1.0f);
    assert(captured.event.rotation.yaw == 45.0f);
    clf4_actor_hook_callback_stub_clear();
    return 0;
}
