#include "commonlibf4_player_hook_source_candidate_adapter.h"
#include "commonlibf4_player_hook_runtime_probe.h"

#include <stdio.h>
#include <string.h>

typedef struct CommonLibF4PlayerHookSourceCandidateAdapterState {
    CommonLibF4PlayerHookSourceCandidateAdapterConfig config;
    bool installed;
    bool callback_registered;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t real_callback_forward_count;
    uint32_t real_callback_accept_count;
    uint32_t real_callback_reject_count;
    uint32_t last_probe_callback_count;
    LocalPlayerObservedState last_state;
    bool has_state;
    CommonLibF4PlayerHookDispatchKind last_dispatch_kind;
} CommonLibF4PlayerHookSourceCandidateAdapterState;

static CommonLibF4PlayerHookSourceCandidateAdapterState g_adapter;

static void append_trace(const char* event_name, bool ok, const CommonLibF4PlayerHookArgs* args, const CommonLibF4PlayerHookRuntimeProbeState* probe) {
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_adapter_trace.txt";
    const bool real_callback_evidence = (g_adapter.real_callback_forward_count > 0u ||
                                         g_adapter.real_callback_accept_count > 0u ||
                                         g_adapter.real_callback_reject_count > 0u);
    FILE* f = NULL;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "event=%s ok=%d dispatch=%s installed=%d callback_registered=%d forward=%u accept=%u reject=%u real_forward=%u real_accept=%u real_reject=%u last_probe_callback_count=%u expected_local_player_id=%u has_state=%d observed_player_id=%u real_callback_evidence=%d probe_callback_count=%u probe_rejected=%u probe_mismatched=%u probe_last_reject_reason=%d",
            event_name ? event_name : "unknown",
            ok ? 1 : 0,
            clf4_phdc_label(g_adapter.last_dispatch_kind),
            g_adapter.installed ? 1 : 0,
            g_adapter.callback_registered ? 1 : 0,
            (unsigned int)g_adapter.callback_forward_count,
            (unsigned int)g_adapter.callback_accept_count,
            (unsigned int)g_adapter.callback_reject_count,
            (unsigned int)g_adapter.real_callback_forward_count,
            (unsigned int)g_adapter.real_callback_accept_count,
            (unsigned int)g_adapter.real_callback_reject_count,
            (unsigned int)g_adapter.last_probe_callback_count,
            (unsigned int)g_adapter.config.expected_local_player_id,
            g_adapter.has_state ? 1 : 0,
            (unsigned int)(g_adapter.has_state ? g_adapter.last_state.player_id : 0),
            real_callback_evidence ? 1 : 0,
            probe ? (unsigned int)probe->callback_count : 0,
            probe ? (unsigned int)probe->rejected_callback_count : 0,
            probe ? (unsigned int)probe->mismatched_player_count : 0,
            probe ? (int)probe->last_reject_reason : 0);
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

static bool source_candidate_cb(const CommonLibF4PlayerHookArgs* args, void* user) {
    CommonLibF4PlayerHookSourceCandidateAdapterState* s = (CommonLibF4PlayerHookSourceCandidateAdapterState*)user;
    CommonLibF4PlayerHookRuntimeProbeState probe;
    CommonLibF4LocalPlayerProbeProviderStats provider_stats;
    CommonLibF4PlayerHookDispatchKind dispatch_kind;
    bool ok;

    if (!s || !s->installed) {
        append_trace("cb_not_installed", false, args, NULL);
        return false;
    }

    dispatch_kind = clf4_phdc_get();
    s->last_dispatch_kind = dispatch_kind;
    s->callback_forward_count++;
    if (dispatch_kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
        s->real_callback_forward_count++;
    }

    ok = clf4_phrp_submit_callback(args);
    probe = clf4_phrp_state();
    s->last_probe_callback_count = probe.callback_count;

    if (ok) {
        provider_stats = clf4_lppp_stats();
        if (provider_stats.has_state) {
            s->last_state = provider_stats.last_state;
            s->has_state = true;
        }
        s->callback_accept_count++;
        if (dispatch_kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
            s->real_callback_accept_count++;
        }
        append_trace("cb_accept", true, args, &probe);
    } else {
        s->callback_reject_count++;
        if (dispatch_kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
            s->real_callback_reject_count++;
        }
        append_trace("cb_reject", false, args, &probe);
    }

    return ok;
}

void clf4_phsca_install(const CommonLibF4PlayerHookSourceCandidateAdapterConfig* config) {
    CommonLibF4LocalPlayerProbeProviderConfig provider_cfg;
    CommonLibF4PlayerHookCallbackRegistration reg;

    memset(&g_adapter, 0, sizeof(g_adapter));
    if (config) {
        g_adapter.config = *config;
    }

    memset(&provider_cfg, 0, sizeof(provider_cfg));
    provider_cfg.expected_local_player_id = g_adapter.config.expected_local_player_id;
    provider_cfg.require_hook_correct = g_adapter.config.require_hook_correct;
    clf4_lppp_install(&provider_cfg);

    memset(&reg, 0, sizeof(reg));
    reg.callback = source_candidate_cb;
    reg.user = &g_adapter;
    clf4_player_hook_callback_stub_register(&reg);

    g_adapter.callback_registered = true;
    g_adapter.installed = true;
    g_adapter.last_dispatch_kind = CLF4_PHDC_DISPATCH_NONE;
    append_trace("install", true, NULL, NULL);
}

void clf4_phsca_uninstall(void) {
    clf4_player_hook_callback_stub_clear();
    clf4_lppp_uninstall();
    append_trace("uninstall", true, NULL, NULL);
    memset(&g_adapter, 0, sizeof(g_adapter));
}

bool clf4_phsca_installed(void) {
    return g_adapter.installed;
}

CommonLibF4PlayerHookSourceCandidateAdapterStats clf4_phsca_stats(void) {
    CommonLibF4PlayerHookSourceCandidateAdapterStats out;
    CommonLibF4LocalPlayerProbeProviderStats provider_stats;
    memset(&out, 0, sizeof(out));
    provider_stats = clf4_lppp_stats();
    out.installed = g_adapter.installed;
    out.callback_registered = g_adapter.callback_registered;
    out.provider_installed = provider_stats.installed;
    out.callback_forward_count = g_adapter.callback_forward_count;
    out.callback_accept_count = g_adapter.callback_accept_count;
    out.callback_reject_count = g_adapter.callback_reject_count;
    out.real_callback_forward_count = g_adapter.real_callback_forward_count;
    out.real_callback_accept_count = g_adapter.real_callback_accept_count;
    out.real_callback_reject_count = g_adapter.real_callback_reject_count;
    out.last_probe_callback_count = g_adapter.last_probe_callback_count;
    out.expected_local_player_id = g_adapter.config.expected_local_player_id;
    out.has_state = provider_stats.has_state || g_adapter.has_state;
    out.last_state = provider_stats.has_state ? provider_stats.last_state : g_adapter.last_state;
    out.last_dispatch_kind = g_adapter.last_dispatch_kind;
    return out;
}
