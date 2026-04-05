#include <assert.h>
#include <string.h>
#include "commonlibf4_workshop_hook_callback_stub.h"

static bool seen_cb(const WorkshopHostEvent* ev, void* user) {
    WorkshopHostEvent* out = (WorkshopHostEvent*)user;
    *out = *ev;
    return true;
}

int main(void) {
    CommonLibF4WorkshopHookCallbackRegistration reg;
    CommonLibF4WorkshopHookCallbackStubStats stats;
    WorkshopHostEvent ev;
    WorkshopHostEvent seen;
    memset(&ev, 0, sizeof(ev));
    memset(&seen, 0, sizeof(seen));

    reg.callback = seen_cb;
    reg.user = &seen;
    clf4_workshop_hook_callback_stub_register(&reg);

    ev.kind = WHE_PLACE;
    ev.object_net_id = 77;
    ev.form_id = 0x1234;
    ev.position.x = 10.0f;
    ev.rotation.yaw = 90.0f;

    assert(clf4_workshop_hook_callback_stub_emit(&ev));
    stats = clf4_workshop_hook_callback_stub_stats();
    assert(stats.registered_callback);
    assert(stats.emit_count >= 1);
    assert(stats.callback_success_count >= 1);
    assert(stats.has_last_event);
    assert(stats.last_event.object_net_id == 77);
    assert(seen.object_net_id == 77);
    assert(seen.position.x == 10.0f);

    clf4_workshop_hook_callback_stub_clear();
    return 0;
}
