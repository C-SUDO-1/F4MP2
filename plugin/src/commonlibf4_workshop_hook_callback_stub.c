#include "commonlibf4_workshop_hook_callback_stub.h"
#include <string.h>

typedef struct CommonLibF4WorkshopHookCallbackStubState {
    CommonLibF4WorkshopHookCallbackRegistration reg;
    CommonLibF4WorkshopHookCallbackStubStats stats;
} CommonLibF4WorkshopHookCallbackStubState;

static CommonLibF4WorkshopHookCallbackStubState g_stub;

void clf4_workshop_hook_callback_stub_register(const CommonLibF4WorkshopHookCallbackRegistration* reg) {
    memset(&g_stub.reg, 0, sizeof(g_stub.reg));
    if (reg) {
        g_stub.reg = *reg;
    }
    g_stub.stats.registered_callback = (g_stub.reg.callback != 0);
    g_stub.stats.register_count++;
}

void clf4_workshop_hook_callback_stub_clear(void) {
    memset(&g_stub.reg, 0, sizeof(g_stub.reg));
    g_stub.stats.registered_callback = false;
}

bool clf4_workshop_hook_callback_stub_emit(const WorkshopHostEvent* ev) {
    bool ok = false;
    if (!ev) return false;
    g_stub.stats.emit_count++;
    g_stub.stats.has_last_event = true;
    g_stub.stats.last_event = *ev;
    if (g_stub.reg.callback) {
        ok = g_stub.reg.callback(ev, g_stub.reg.user);
        if (ok) g_stub.stats.callback_success_count++;
        else g_stub.stats.callback_failure_count++;
        return ok;
    }
    g_stub.stats.callback_failure_count++;
    return false;
}

CommonLibF4WorkshopHookCallbackStubStats clf4_workshop_hook_callback_stub_stats(void) {
    return g_stub.stats;
}
