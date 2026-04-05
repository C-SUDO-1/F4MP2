#include "commonlibf4_player_hook_callback_stub.h"
#include "engine_bridge_stub.h"

#include <stdio.h>
#include <string.h>

typedef struct CommonLibF4PlayerHookCallbackStubState {
    CommonLibF4PlayerHookCallbackRegistration reg;
    CommonLibF4PlayerHookCallbackStubStats stats;
} CommonLibF4PlayerHookCallbackStubState;

static CommonLibF4PlayerHookCallbackStubState g_stub;

static void append_real_entry_trace(const CommonLibF4PlayerHookArgs* args, bool ok) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_real_entry_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=real_emit ok=%d real_emit_count=%u real_success=%u real_failure=%u registered=%d",
            ok ? 1 : 0,
            g_stub.stats.real_emit_count,
            g_stub.stats.real_callback_success_count,
            g_stub.stats.real_callback_failure_count,
            g_stub.stats.registered_callback ? 1 : 0);

    if (args) {
        fprintf(f,
                " player_id=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
                (unsigned int)args->event.player_id,
                (unsigned int)args->event.stance,
                args->event.position.x,
                args->event.position.y,
                args->event.position.z,
                args->event.rotation.yaw,
                args->event.velocity.x,
                args->event.velocity.y,
                args->event.velocity.z);
    }

    fputc('\n', f);
    fclose(f);
}

static void append_trace(const char* event_name, const CommonLibF4PlayerHookArgs* args, bool ok) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_callback_trace.txt";

    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s ok=%d dispatch=%s registered=%d register_count=%u emit_count=%u synthetic_emit_count=%u real_emit_count=%u success_count=%u failure_count=%u real_success_count=%u real_failure_count=%u",
            event_name ? event_name : "unknown",
            ok ? 1 : 0,
            clf4_phdc_label(g_stub.stats.last_dispatch_kind),
            g_stub.stats.registered_callback ? 1 : 0,
            g_stub.stats.register_count,
            g_stub.stats.emit_count,
            g_stub.stats.synthetic_emit_count,
            g_stub.stats.real_emit_count,
            g_stub.stats.callback_success_count,
            g_stub.stats.callback_failure_count,
            g_stub.stats.real_callback_success_count,
            g_stub.stats.real_callback_failure_count);

    if (args) {
        fprintf(f,
                " player_id=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
                (unsigned int)args->event.player_id,
                (unsigned int)args->event.stance,
                args->event.position.x,
                args->event.position.y,
                args->event.position.z,
                args->event.rotation.yaw,
                args->event.velocity.x,
                args->event.velocity.y,
                args->event.velocity.z);
    }

    fputc('\n', f);
    fclose(f);
}

static bool dispatch_emit(const CommonLibF4PlayerHookArgs* args,
                          CommonLibF4PlayerHookDispatchKind kind,
                          const char* event_name,
                          bool tick_after_dispatch) {
    bool ok = false;

    if (!args) {
        g_stub.stats.last_dispatch_kind = kind;
        append_trace(kind == CLF4_PHDC_DISPATCH_REAL_HOOK ? "real_emit_null_args" : "emit_null_args", NULL, false);
        return false;
    }

    g_stub.stats.emit_count++;
    g_stub.stats.has_last_args = true;
    g_stub.stats.last_args = *args;
    g_stub.stats.last_dispatch_kind = kind;
    if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
        g_stub.stats.real_emit_count++;
    } else if (kind == CLF4_PHDC_DISPATCH_SYNTHETIC_VALIDATION) {
        g_stub.stats.synthetic_emit_count++;
    }

    clf4_phdc_set(kind);
    if (g_stub.reg.callback) {
        ok = g_stub.reg.callback(args, g_stub.reg.user);
        if (ok) {
            g_stub.stats.callback_success_count++;
            if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
                g_stub.stats.real_callback_success_count++;
            }
        } else {
            g_stub.stats.callback_failure_count++;
            if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
                g_stub.stats.real_callback_failure_count++;
            }
        }
    } else {
        g_stub.stats.callback_failure_count++;
        if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
            g_stub.stats.real_callback_failure_count++;
        }
    }
    clf4_phdc_reset();

    if (tick_after_dispatch) {
        (void)ebstub_live_tick(NULL, NULL);
    }

    append_trace(event_name, args, ok);
    if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
        append_real_entry_trace(args, ok);
    }
    return ok;
}

void clf4_player_hook_callback_stub_register(const CommonLibF4PlayerHookCallbackRegistration* reg) {
    memset(&g_stub.reg, 0, sizeof(g_stub.reg));
    if (reg) {
        g_stub.reg = *reg;
    }
    g_stub.stats.registered_callback = (g_stub.reg.callback != 0);
    g_stub.stats.register_count++;
    g_stub.stats.last_dispatch_kind = CLF4_PHDC_DISPATCH_NONE;
    append_trace("register", NULL, g_stub.stats.registered_callback);
}

void clf4_player_hook_callback_stub_clear(void) {
    memset(&g_stub.reg, 0, sizeof(g_stub.reg));
    g_stub.stats.registered_callback = false;
    g_stub.stats.last_dispatch_kind = CLF4_PHDC_DISPATCH_NONE;
    clf4_phdc_reset();
    append_trace("clear", NULL, true);
}

bool clf4_player_hook_callback_stub_emit(const CommonLibF4PlayerHookArgs* args) {
    return dispatch_emit(args, CLF4_PHDC_DISPATCH_SYNTHETIC_VALIDATION, "emit", true);
}

bool clf4_player_hook_callback_real_emit(const CommonLibF4PlayerHookArgs* args) {
    return dispatch_emit(args, CLF4_PHDC_DISPATCH_REAL_HOOK, "real_emit", false);
}

CommonLibF4PlayerHookCallbackStubStats clf4_player_hook_callback_stub_stats(void) {
    return g_stub.stats;
}
