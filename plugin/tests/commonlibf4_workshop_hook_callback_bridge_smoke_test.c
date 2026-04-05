#include <assert.h>
#include <string.h>
#include "commonlibf4_workshop_hook_callback_stub.h"
#include "live_hook_pipeline_stub.h"
#include "proxy_runtime.h"
#include "engine_bridge_stub.h"

static bool bridge_cb(const WorkshopHostEvent* ev, void* user) {
    (void)user;
    return lhps_submit_workshop_event(ev);
}

int main(void) {
    CommonLibF4WorkshopHookCallbackRegistration reg;
    CommonLibF4WorkshopHookCallbackStubStats stats;
    WorkshopHostEvent ev;
    EngineBridgeObjectSnapshot obj;
    memset(&ev, 0, sizeof(ev));
    memset(&obj, 0, sizeof(obj));

    proxy_runtime_init();
    ebstub_init();
    reg.callback = bridge_cb;
    reg.user = 0;
    clf4_workshop_hook_callback_stub_register(&reg);

    ev.kind = WHE_PLACE;
    ev.object_net_id = 1;
    ev.form_id = 0xABCD;
    ev.position.x = 3.0f;
    ev.position.y = 4.0f;
    ev.rotation.yaw = 45.0f;
    ev.state_flags = 9;

    assert(clf4_workshop_hook_callback_stub_emit(&ev));
    stats = clf4_workshop_hook_callback_stub_stats();
    assert(stats.callback_success_count >= 1);

    assert(ebstub_object_count() >= 1);
    assert(ebstub_get_object_snapshot(0, &obj));
    assert(obj.object_net_id == 1);
    assert(obj.form_id == 0xABCD);
    assert(obj.position.x == 3.0f);

    clf4_workshop_hook_callback_stub_clear();
    return 0;
}
