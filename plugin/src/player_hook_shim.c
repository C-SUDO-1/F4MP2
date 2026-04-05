#include "player_hook_shim.h"
#include "commonlibf4_player_hook_callback_stub.h"
#include "fo4_bridge_ingress.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#endif

static MsgPlayerState g_last_player_snapshot;
static int g_has_last = 0;
static unsigned int g_ingest_count = 0;
static unsigned int g_real_entry_count = 0;
static unsigned int g_detour_entry_count = 0;
static unsigned int g_detour_bridge_forward_count = 0;
static unsigned int g_real_dispatch_attempt_count = 0;
static unsigned int g_real_dispatch_success_count = 0;
static unsigned int g_real_dispatch_failure_count = 0;
static unsigned int g_last_thread_id = 0;
static uintptr_t g_last_return_address = 0u;
static int g_has_last_return_address = 0;
static uintptr_t g_manual_create_player_controls_trampoline = 0u;

static unsigned int current_thread_id_value(void) {
#ifdef _WIN32
    return (unsigned int)GetCurrentThreadId();
#else
    return (unsigned int)((uintptr_t)pthread_self() & 0xFFFFFFFFu);
#endif
}

static uintptr_t current_return_address_value(void) {
#ifdef _MSC_VER
    return (uintptr_t)_ReturnAddress();
#else
    return (uintptr_t)0u;
#endif
}

static void update_entry_origin_snapshot(void) {
    g_last_thread_id = current_thread_id_value();
    g_last_return_address = current_return_address_value();
    g_has_last_return_address = (g_last_return_address != 0u) ? 1 : 0;
}

static void real_entry_probe(void) {
    FILE* f = fopen(
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_real_entry.txt",
        "a");
    if (f) {
        fprintf(f, "REAL ENTRY HIT count=%u\n", g_real_entry_count);
        fclose(f);
    }
}

static void append_trace(const char* event_name,
                         const MsgPlayerState* msg,
                         int forwarded,
                         int real_dispatch_attempted,
                         int real_dispatch_ok) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_player_shim_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s ingest_count=%u real_entry_count=%u detour_entry_count=%u detour_bridge_forward_count=%u has_last=%d forwarded=%d real_dispatch_attempted=%d real_dispatch_ok=%d real_dispatch_attempts=%u real_dispatch_success=%u real_dispatch_failure=%u last_thread_id=%u has_last_return_address=%d last_return_address=0x%p",
            event_name ? event_name : "unknown",
            g_ingest_count,
            g_real_entry_count,
            g_detour_entry_count,
            g_detour_bridge_forward_count,
            g_has_last,
            forwarded,
            real_dispatch_attempted,
            real_dispatch_ok,
            g_real_dispatch_attempt_count,
            g_real_dispatch_success_count,
            g_real_dispatch_failure_count,
            g_last_thread_id,
            g_has_last_return_address,
            (void*)g_last_return_address);

    if (msg) {
        fprintf(f,
                " player_id=%u seq=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
                (unsigned int)msg->player_id,
                (unsigned int)msg->state_sequence,
                (unsigned int)msg->stance,
                msg->position.x,
                msg->position.y,
                msg->position.z,
                msg->rotation.yaw,
                msg->velocity.x,
                msg->velocity.y,
                msg->velocity.z);
    }

    fputc('\n', f);
    fclose(f);
}

static void append_real_bridge_trace(const char* event_name,
                                     const MsgPlayerState* msg,
                                     int real_dispatch_ok,
                                     int forwarded) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_real_entry_bridge_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s real_entry_count=%u detour_entry_count=%u detour_bridge_forward_count=%u real_dispatch_attempts=%u real_dispatch_success=%u real_dispatch_failure=%u real_dispatch_ok=%d forwarded=%d last_thread_id=%u has_last_return_address=%d last_return_address=0x%p",
            event_name ? event_name : "unknown",
            g_real_entry_count,
            g_detour_entry_count,
            g_detour_bridge_forward_count,
            g_real_dispatch_attempt_count,
            g_real_dispatch_success_count,
            g_real_dispatch_failure_count,
            real_dispatch_ok,
            forwarded,
            g_last_thread_id,
            g_has_last_return_address,
            (void*)g_last_return_address);

    if (msg) {
        fprintf(f,
                " player_id=%u seq=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
                (unsigned int)msg->player_id,
                (unsigned int)msg->state_sequence,
                (unsigned int)msg->stance,
                msg->position.x,
                msg->position.y,
                msg->position.z,
                msg->rotation.yaw,
                msg->velocity.x,
                msg->velocity.y,
                msg->velocity.z);
    }

    fputc('\n', f);
    fclose(f);
}

static void append_detour_entry_trace(const char* event_name,
                                      const char* site_label,
                                      int bridge_forwarded,
                                      int real_dispatch_attempted,
                                      int real_dispatch_ok) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_detour_entry_trace.txt";
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s site=%s detour_entry_count=%u detour_bridge_forward_count=%u real_entry_count=%u bridge_forwarded=%d real_dispatch_attempted=%d real_dispatch_ok=%d real_dispatch_attempts=%u real_dispatch_success=%u real_dispatch_failure=%u last_thread_id=%u has_last_return_address=%d last_return_address=0x%p",
            event_name ? event_name : "unknown",
            site_label ? site_label : "",
            g_detour_entry_count,
            g_detour_bridge_forward_count,
            g_real_entry_count,
            bridge_forwarded,
            real_dispatch_attempted,
            real_dispatch_ok,
            g_real_dispatch_attempt_count,
            g_real_dispatch_success_count,
            g_real_dispatch_failure_count,
            g_last_thread_id,
            g_has_last_return_address,
            (void*)g_last_return_address);
    fputc('\n', f);
    fclose(f);
}

static void fill_real_hook_args(const MsgPlayerState* msg, CommonLibF4PlayerHookArgs* out_args) {
    memset(out_args, 0, sizeof(*out_args));
    out_args->event.player_id = msg->player_id;
    out_args->event.position = msg->position;
    out_args->event.rotation = msg->rotation;
    out_args->event.velocity = msg->velocity;
    out_args->event.stance = msg->stance;
    out_args->event.anim_state = msg->anim_state;
    out_args->event.equipped_form_id = msg->equipped_form_id;
}

void phs_init(void) {
    memset(&g_last_player_snapshot, 0, sizeof(g_last_player_snapshot));
    g_has_last = 0;
    g_ingest_count = 0;
    g_real_entry_count = 0;
    g_detour_entry_count = 0;
    g_detour_bridge_forward_count = 0;
    g_real_dispatch_attempt_count = 0;
    g_real_dispatch_success_count = 0;
    g_real_dispatch_failure_count = 0;
    g_last_thread_id = 0;
    g_last_return_address = 0u;
    g_has_last_return_address = 0;
    g_manual_create_player_controls_trampoline = 0u;
    append_trace("init", NULL, 1, 0, 0);
    append_detour_entry_trace("init", "", 0, 0, 0);
}

void phs_set_manual_create_player_controls_trampoline(uintptr_t trampoline_addr) {
    g_manual_create_player_controls_trampoline = trampoline_addr;
}

uintptr_t phs_get_manual_create_player_controls_trampoline(void) {
    return g_manual_create_player_controls_trampoline;
}

uintptr_t phs_create_player_controls_manual_detour_thunk(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4) {
    typedef uintptr_t (*ManualCreatePlayerControlsThunkFn)(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
    ManualCreatePlayerControlsThunkFn trampoline_fn = (ManualCreatePlayerControlsThunkFn)g_manual_create_player_controls_trampoline;

    /* Slice 3J: pure pass-through fallback.
       This function should never be used on the live detour path once the
       patch realizer switches to the executable pass-through gateway. Keep it
       as a direct trampoline jump with no bookkeeping so it remains a benign
       fallback if referenced elsewhere. */
    if (trampoline_fn) {
        return trampoline_fn(a1, a2, a3, a4);
    }

    return (uintptr_t)0u;
}

void phs_note_manual_detour_entry(const char* site_label) {
    update_entry_origin_snapshot();
    g_detour_entry_count++;
    append_detour_entry_trace("manual_detour_entry", site_label, 0, 0, 0);
}

bool phs_ingest_real_player_snapshot(const MsgPlayerState* msg) {
    bool forwarded = false;
    bool real_dispatch_ok = false;
    CommonLibF4PlayerHookArgs hook_args;

    update_entry_origin_snapshot();
    g_detour_bridge_forward_count++;
    append_detour_entry_trace("shim_real_ingest_enter", "player_hook_shim", 1, 0, 0);

    if (!msg) {
        append_trace("real_ingest_null", NULL, 0, 0, 0);
        append_real_bridge_trace("real_ingest_null", NULL, 0, 0);
        append_detour_entry_trace("shim_real_ingest_null", "player_hook_shim", 1, 0, 0);
        return false;
    }

    g_real_entry_count++;
    real_entry_probe();

    g_last_player_snapshot = *msg;
    g_has_last = 1;
    g_ingest_count++;

    fill_real_hook_args(msg, &hook_args);
    g_real_dispatch_attempt_count++;
    real_dispatch_ok = clf4_player_hook_callback_real_emit(&hook_args);
    if (real_dispatch_ok) {
        g_real_dispatch_success_count++;
    } else {
        g_real_dispatch_failure_count++;
    }

    forwarded = fbi_ingest_remote_player_state(msg);
    append_trace("real_ingest", msg, forwarded ? 1 : 0, 1, real_dispatch_ok ? 1 : 0);
    append_real_bridge_trace("real_ingest", msg, real_dispatch_ok ? 1 : 0, forwarded ? 1 : 0);
    append_detour_entry_trace("shim_real_ingest_exit", "player_hook_shim", forwarded ? 1 : 0, 1, real_dispatch_ok ? 1 : 0);
    return real_dispatch_ok || forwarded;
}

bool phs_ingest_player_snapshot(const MsgPlayerState* msg) {
    return phs_ingest_real_player_snapshot(msg);
}

bool phs_get_last_player_snapshot(MsgPlayerState* out_msg) {
    if (!out_msg || !g_has_last) {
        append_trace("get_last_unavailable", NULL, 0, 0, 0);
        return false;
    }
    *out_msg = g_last_player_snapshot;
    append_trace("get_last", &g_last_player_snapshot, 1, 0, 0);
    return true;
}

PlayerHookShimStats phs_stats(void) {
    PlayerHookShimStats out;
    memset(&out, 0, sizeof(out));
    out.ingest_count = g_ingest_count;
    out.real_entry_count = g_real_entry_count;
    out.detour_entry_count = g_detour_entry_count;
    out.detour_bridge_forward_count = g_detour_bridge_forward_count;
    out.real_dispatch_attempt_count = g_real_dispatch_attempt_count;
    out.real_dispatch_success_count = g_real_dispatch_success_count;
    out.real_dispatch_failure_count = g_real_dispatch_failure_count;
    out.last_thread_id = g_last_thread_id;
    out.last_return_address = g_last_return_address;
    out.has_last_return_address = g_has_last_return_address ? true : false;
    return out;
}
