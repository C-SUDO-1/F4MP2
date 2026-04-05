#include "commonlibf4_player_live_adapter.h"
#include "commonlibf4_player_hook_dispatch_context.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

static CommonLibF4PlayerLiveAdapterStats g_stats;

static const char* trace_path_for_dispatch(CommonLibF4PlayerHookDispatchKind kind) {
    if (kind == CLF4_PHDC_DISPATCH_REAL_HOOK) {
        return "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_real_hit.txt";
    }
    return "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_player_live_adapter_trace.txt";
}

static void append_trace(const char* event_name,
                         const CommonLibF4PlayerHookArgs* args,
                         const CommonLibF4PlayerLiveSample* sample,
                         int ok) {
    const CommonLibF4PlayerHookDispatchKind dispatch_kind = clf4_phdc_get();
    FILE* f = fopen(trace_path_for_dispatch(dispatch_kind), "a");
    if (!f) {
        return;
    }

    fprintf(
        f,
        "event=%s dispatch=%s ok=%d normalize_attempt=%u accept_count=%u reject_count=%u last_reject=%d fallback_player_id=%u",
        event_name ? event_name : "unknown",
        clf4_phdc_label(dispatch_kind),
        ok,
        (unsigned)g_stats.normalize_attempt_count,
        (unsigned)g_stats.accept_count,
        (unsigned)g_stats.reject_count,
        (int)g_stats.last_reject_reason,
        (unsigned)g_stats.fallback_player_id);

    if (args) {
        fprintf(
            f,
            " arg_player=%u stance=%u pos=(%.3f,%.3f,%.3f) yaw=%.3f vel=(%.3f,%.3f,%.3f)",
            (unsigned)args->event.player_id,
            (unsigned)args->event.stance,
            args->event.position.x, args->event.position.y, args->event.position.z,
            args->event.rotation.yaw,
            args->event.velocity.x, args->event.velocity.y, args->event.velocity.z);
    }

    if (sample) {
        fprintf(
            f,
            " sample_player=%u seq=%u tick=%u default_id=%u",
            (unsigned)sample->state.player_id,
            (unsigned)sample->accepted_sequence,
            (unsigned)sample->observed_tick,
            (unsigned)(sample->used_default_player_id ? 1 : 0));
    }

    fputc('\n', f);
    fclose(f);
}

static bool vec3_finite(Vec3f v) {
    return isfinite(v.x) && isfinite(v.y) && isfinite(v.z);
}

static bool rot3_finite(Rot3f r) {
    return isfinite(r.pitch) && isfinite(r.yaw) && isfinite(r.roll);
}

void clf4_pla_reset(PlayerId fallback_player_id) {
    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.fallback_player_id = fallback_player_id;
    append_trace("reset", NULL, NULL, 1);
}

bool clf4_pla_normalize(const CommonLibF4PlayerHookArgs* args, CommonLibF4PlayerLiveSample* out_sample) {
    CommonLibF4PlayerLiveSample sample;
    PlayerId resolved_player_id;

    g_stats.normalize_attempt_count++;
    append_trace("normalize_enter", args, NULL, 1);

    if (!args) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_NULL_ARGS;
        append_trace("reject_null_args", NULL, NULL, 0);
        return false;
    }
    if (!out_sample) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_NULL_OUT;
        append_trace("reject_null_out", args, NULL, 0);
        return false;
    }

    resolved_player_id = args->event.player_id ? args->event.player_id : g_stats.fallback_player_id;
    if (resolved_player_id == 0) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_MISSING_PLAYER_ID;
        append_trace("reject_missing_player_id", args, NULL, 0);
        return false;
    }
    if (!vec3_finite(args->event.position)) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_INVALID_POSITION;
        append_trace("reject_invalid_position", args, NULL, 0);
        return false;
    }
    if (!rot3_finite(args->event.rotation)) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_INVALID_ROTATION;
        append_trace("reject_invalid_rotation", args, NULL, 0);
        return false;
    }
    if (!vec3_finite(args->event.velocity)) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_INVALID_VELOCITY;
        append_trace("reject_invalid_velocity", args, NULL, 0);
        return false;
    }
    if (args->event.stance > STANCE_FALL) {
        g_stats.reject_count++;
        g_stats.last_reject_reason = CLF4_PLA_REJECT_INVALID_STANCE;
        append_trace("reject_invalid_stance", args, NULL, 0);
        return false;
    }

    memset(&sample, 0, sizeof(sample));
    sample.state.valid = true;
    sample.state.player_id = resolved_player_id;
    sample.state.position = args->event.position;
    sample.state.rotation = args->event.rotation;
    sample.state.velocity = args->event.velocity;
    sample.state.stance = args->event.stance;
    sample.state.anim_state = args->event.anim_state;
    sample.state.equipped_form_id = args->event.equipped_form_id;
    sample.source_kind = (uint8_t)CLF4_PLA_SOURCE_HOOK_CALLBACK;
    sample.used_default_player_id = (args->event.player_id == 0);
    sample.accepted_sequence = g_stats.accept_count + 1u;
    sample.observed_tick = g_stats.last_observed_tick + 1u;

    *out_sample = sample;
    g_stats.accept_count++;
    g_stats.last_observed_tick = sample.observed_tick;
    g_stats.last_accepted_sequence = sample.accepted_sequence;
    g_stats.has_last_sample = true;
    g_stats.last_reject_reason = CLF4_PLA_REJECT_NONE;
    g_stats.last_sample = sample;
    append_trace("accept", args, &sample, 1);
    return true;
}

CommonLibF4PlayerLiveAdapterStats clf4_pla_stats(void) {
    return g_stats;
}
