#include "player_hook_probe_harvest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHH_MAX_CLUSTERS 32u

typedef struct PlayerHookProbeHarvestCluster {
    uintptr_t representative_addr;
    uintptr_t caller_addr;
    uint32_t sample_count;
    uint32_t synthetic_sample_count;
    uint32_t movement_hits;
    uint32_t idle_hits;
    uint32_t local_player_hits;
    uint32_t score;
    bool promoted;
} PlayerHookProbeHarvestCluster;

typedef struct PlayerHookProbeHarvestState {
    uint32_t next_session_id;
    uint32_t session_id;
    bool active;
    char family[96];
    char reason[64];
    char mode[32];
    char scenario_tag[32];
    uint32_t sample_count;
    uint32_t accepted_sample_count;
    uint32_t synthetic_sample_count;
    uint32_t cluster_count;
    uint32_t promoted_candidate_count;
    PlayerHookProbeHarvestCluster clusters[PHH_MAX_CLUSTERS];
} PlayerHookProbeHarvestState;

static PlayerHookProbeHarvestState g_state;

static const char* HARVEST_TRACE_PATH =
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_probe_harvest_trace.txt";
static const char* CALLSITE_TRACE_PATH =
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_callsite_harvest_trace.txt";
static const char* CLUSTER_TRACE_PATH =
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_candidate_cluster_trace.txt";
static const char* PROMOTION_TRACE_PATH =
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_discovered_candidate_promotion_trace.txt";

static const char* env_or_default(const char* name, const char* fallback) {
    const char* v = getenv(name);
    return (v && *v) ? v : fallback;
}

static uintptr_t capture_return_addr(void) {
#if defined(__GNUC__) || defined(__clang__)
    return (uintptr_t)__builtin_return_address(0);
#else
    return 0u;
#endif
}

static uintptr_t capture_caller_addr(void) {
#if defined(__GNUC__) || defined(__clang__)
    return (uintptr_t)__builtin_return_address(1);
#else
    return 0u;
#endif
}

static uint32_t compute_score(const PlayerHookProbeHarvestCluster* c) {
    uint32_t score = 0u;
    if (!c) {
        return 0u;
    }
    score += c->movement_hits * 10u;
    score += c->local_player_hits * 5u;
    if (c->idle_hits < c->movement_hits) {
        score += (c->movement_hits - c->idle_hits) * 4u;
    }
    if (c->synthetic_sample_count > 0u) {
        uint32_t penalty = c->synthetic_sample_count * 8u;
        score = (penalty > score) ? 0u : (score - penalty);
    }
    return score;
}

static void append_probe_trace(const char* event_name) {
    FILE* f = NULL;
    fopen_s(&f, HARVEST_TRACE_PATH, "a");
    if (!f) {
        return;
    }
    fprintf(f,
            "event=%s session_id=%u active=%d family=%s reason=%s mode=%s scenario=%s sample_count=%u accepted_sample_count=%u synthetic_sample_count=%u cluster_count=%u promoted_candidate_count=%u\n",
            event_name ? event_name : "unknown",
            (unsigned int)g_state.session_id,
            g_state.active ? 1 : 0,
            g_state.family,
            g_state.reason,
            g_state.mode,
            g_state.scenario_tag,
            (unsigned int)g_state.sample_count,
            (unsigned int)g_state.accepted_sample_count,
            (unsigned int)g_state.synthetic_sample_count,
            (unsigned int)g_state.cluster_count,
            (unsigned int)g_state.promoted_candidate_count);
    fclose(f);
}

static void append_callsite_trace(const char* seam_name,
                                  uintptr_t return_addr,
                                  uintptr_t caller_addr,
                                  bool synthetic_event,
                                  bool local_player_valid,
                                  bool movement_context_valid,
                                  uint32_t sample_index,
                                  uint32_t hit_count) {
    FILE* f = NULL;
    fopen_s(&f, CALLSITE_TRACE_PATH, "a");
    if (!f) {
        return;
    }
    fprintf(f,
            "session_id=%u sample_index=%u seam=%s return_addr=0x%p caller_addr=0x%p thread_id=0 module_rva=0x0 scenario_tag=%s local_player_valid=%d movement_context_valid=%d synthetic_event=%d hit_count=%u\n",
            (unsigned int)g_state.session_id,
            (unsigned int)sample_index,
            seam_name ? seam_name : "",
            (void*)return_addr,
            (void*)caller_addr,
            g_state.scenario_tag,
            local_player_valid ? 1 : 0,
            movement_context_valid ? 1 : 0,
            synthetic_event ? 1 : 0,
            (unsigned int)hit_count);
    fclose(f);
}

static void append_cluster_trace(uint32_t cluster_id, const PlayerHookProbeHarvestCluster* c) {
    FILE* f = NULL;
    fopen_s(&f, CLUSTER_TRACE_PATH, "a");
    if (!f || !c) {
        if (f) fclose(f);
        return;
    }
    fprintf(f,
            "cluster_id=%u representative_rva=0x%p caller_addr=0x%p sample_count=%u idle_hits=%u movement_hits=%u local_player_hits=%u synthetic_hits=%u score=%u rejection_reason=%s\n",
            (unsigned int)cluster_id,
            (void*)c->representative_addr,
            (void*)c->caller_addr,
            (unsigned int)c->sample_count,
            (unsigned int)c->idle_hits,
            (unsigned int)c->movement_hits,
            (unsigned int)c->local_player_hits,
            (unsigned int)c->synthetic_sample_count,
            (unsigned int)c->score,
            c->sample_count == 0u ? "empty" : (c->score == 0u ? "synthetic_or_idle_noisy" : "none"));
    fclose(f);
}

static void append_promotion_trace(const char* name,
                                   uintptr_t addr,
                                   uint32_t score,
                                   bool allowed,
                                   const char* reason) {
    FILE* f = NULL;
    fopen_s(&f, PROMOTION_TRACE_PATH, "a");
    if (!f) {
        return;
    }
    fprintf(f,
            "candidate_name=%s source_family=%s discovered_rva=0x%p score=%u promotion_reason=%s promotion_allowed=%d promotion_rejected_reason=%s\n",
            name ? name : "",
            g_state.family,
            (void*)addr,
            (unsigned int)score,
            allowed ? "movement_correlated_cluster" : "none",
            allowed ? 1 : 0,
            allowed ? "" : (reason ? reason : "not_promoted"));
    fclose(f);
}

static PlayerHookProbeHarvestCluster* upsert_cluster(uintptr_t return_addr, uintptr_t caller_addr) {
    uint32_t i;
    PlayerHookProbeHarvestCluster* first_empty = NULL;
    for (i = 0; i < PHH_MAX_CLUSTERS; ++i) {
        PlayerHookProbeHarvestCluster* c = &g_state.clusters[i];
        if (c->sample_count == 0u && !first_empty) {
            first_empty = c;
        }
        if (c->sample_count > 0u && c->representative_addr == return_addr) {
            return c;
        }
    }
    if (first_empty) {
        memset(first_empty, 0, sizeof(*first_empty));
        first_empty->representative_addr = return_addr;
        first_empty->caller_addr = caller_addr;
        g_state.cluster_count++;
        return first_empty;
    }
    return &g_state.clusters[PHH_MAX_CLUSTERS - 1u];
}

static void refresh_clusters(void) {
    uint32_t i;
    uint32_t promoted = 0u;
    for (i = 0; i < PHH_MAX_CLUSTERS; ++i) {
        PlayerHookProbeHarvestCluster* c = &g_state.clusters[i];
        if (c->sample_count == 0u) {
            continue;
        }
        c->score = compute_score(c);
        c->promoted = (c->representative_addr != 0u && c->score >= 10u && c->synthetic_sample_count == 0u);
        if (c->promoted) {
            promoted++;
            append_promotion_trace(promoted == 1u ? "movement_playercharacter_before_input_discovered_1" :
                                                  "movement_playercharacter_before_input_discovered_2",
                                  c->representative_addr,
                                  c->score,
                                  true,
                                  NULL);
        } else {
            append_promotion_trace("", c->representative_addr, c->score, false,
                                  c->synthetic_sample_count > 0u ? "synthetic_only" : "low_score");
        }
        append_cluster_trace(i + 1u, c);
    }
    g_state.promoted_candidate_count = promoted;
}

void phh_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
    g_state.next_session_id = 1u;
    snprintf(g_state.mode, sizeof(g_state.mode), "%s", "idle");
    append_probe_trace("reset");
}

void phh_begin_unresolved_session(const HookDiscoveryReport* report) {
    if (g_state.next_session_id == 0u) {
        g_state.next_session_id = 1u;
    }
    if (!report) {
        return;
    }
    g_state.session_id = g_state.next_session_id++;
    g_state.active = true;
    memset(g_state.clusters, 0, sizeof(g_state.clusters));
    g_state.sample_count = 0u;
    g_state.accepted_sample_count = 0u;
    g_state.synthetic_sample_count = 0u;
    g_state.cluster_count = 0u;
    g_state.promoted_candidate_count = 0u;
    snprintf(g_state.family, sizeof(g_state.family), "%s",
             report->probe_family ? report->probe_family : "PlayerCharacter::Update");
    snprintf(g_state.reason, sizeof(g_state.reason), "%s",
             report->probe_reason ? report->probe_reason : "unresolved_required_address");
    snprintf(g_state.mode, sizeof(g_state.mode), "%s",
             report->probe_mode ? report->probe_mode : "probe_only");
    snprintf(g_state.scenario_tag, sizeof(g_state.scenario_tag), "%s",
             env_or_default("F4MP_PROBE_SCENARIO", "idle"));
    append_probe_trace("begin_probe_session");
}

void phh_note_explicit_callsite(const char* seam_name,
                                uintptr_t return_addr,
                                uintptr_t caller_addr,
                                bool synthetic_event,
                                bool local_player_valid,
                                bool movement_context_valid) {
    PlayerHookProbeHarvestCluster* c;
    if (!g_state.active) {
        return;
    }
    c = upsert_cluster(return_addr, caller_addr);
    if (!c) {
        return;
    }
    g_state.sample_count++;
    if (synthetic_event) {
        g_state.synthetic_sample_count++;
        c->synthetic_sample_count++;
    } else {
        g_state.accepted_sample_count++;
    }
    c->sample_count++;
    if (movement_context_valid) {
        c->movement_hits++;
    } else {
        c->idle_hits++;
    }
    if (local_player_valid) {
        c->local_player_hits++;
    }
    append_callsite_trace(seam_name,
                          return_addr,
                          caller_addr,
                          synthetic_event,
                          local_player_valid,
                          movement_context_valid,
                          g_state.sample_count,
                          c->sample_count);
    refresh_clusters();
}

void phh_note_reachable_seam(const char* seam_name,
                            bool synthetic_event,
                            bool local_player_valid,
                            bool movement_context_valid) {
    phh_note_explicit_callsite(seam_name,
                               capture_return_addr(),
                               capture_caller_addr(),
                               synthetic_event,
                               local_player_valid,
                               movement_context_valid);
}

PlayerHookProbeHarvestSnapshot phh_snapshot(void) {
    PlayerHookProbeHarvestSnapshot s;
    memset(&s, 0, sizeof(s));
    s.session_id = g_state.session_id;
    s.active = g_state.active;
    s.family = g_state.family;
    s.reason = g_state.reason;
    s.mode = g_state.mode;
    s.sample_count = g_state.sample_count;
    s.accepted_sample_count = g_state.accepted_sample_count;
    s.synthetic_sample_count = g_state.synthetic_sample_count;
    s.cluster_count = g_state.cluster_count;
    s.promoted_candidate_count = g_state.promoted_candidate_count;
    return s;
}

void phh_refresh_report(HookDiscoveryReport* report) {
    if (!report) {
        return;
    }
    report->probe_session_id = g_state.session_id;
    report->probe_active = g_state.active;
    report->probe_family = g_state.family;
    report->probe_reason = g_state.reason;
    report->probe_mode = g_state.mode;
    report->harvest_sample_count = g_state.sample_count;
    report->harvest_cluster_count = g_state.cluster_count;
    report->promoted_candidate_count = g_state.promoted_candidate_count;
}
