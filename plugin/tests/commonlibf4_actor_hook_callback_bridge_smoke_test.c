#include <assert.h>
#include <string.h>
#include "commonlibf4_actor_hook_callback_stub.h"
#include "commonlibf4_hook_adapter_stub.h"
#include "engine_bridge_stub.h"
#include "proxy_runtime.h"

static bool bridge_cb(const CommonLibF4ActorHookArgs* args, void* user) {
    (void)user;
    return clf4_submit_actor_hook(args);
}

int main(void) {
    CommonLibF4ActorHookArgs in;
    CommonLibF4ActorHookCallbackRegistration reg;
    EngineBridgeActorSnapshot snap;
    memset(&in, 0, sizeof(in));
    memset(&snap, 0, sizeof(snap));

    proxy_runtime_init();
    ebstub_init();
    clf4_hook_adapter_init();
    reg.callback = bridge_cb;
    reg.user = 0;
    clf4_actor_hook_callback_stub_register(&reg);

    in.event.actor_net_id = 9;
    in.event.position.x = 11.0f;
    in.event.rotation.yaw = 270.0f;
    in.event.health_norm = 0.5f;
    in.event.anim_state = 3;
    in.event.actor_flags = ACTF_HOSTILE;

    assert(clf4_actor_hook_callback_stub_emit(&in));
    assert(ebstub_actor_count() == 1);
    assert(ebstub_get_actor_snapshot(0, &snap));
    assert(snap.actor_net_id == 9);
    assert(snap.position.x == 11.0f);
    assert(snap.rotation.yaw == 270.0f);
    assert(snap.health_norm == 0.5f);

    clf4_actor_hook_callback_stub_clear();
    return 0;
}
