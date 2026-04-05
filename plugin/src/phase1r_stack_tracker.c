#include "phase1r_stack_tracker.h"

static const Phase1RStackEntry g_entries[] = {
    { "goal0_harness_join_local_id", PHASE1R_STACK_DONE },
    { "goal0_real_seams_roundtrip", PHASE1R_STACK_DONE },
    { "same_process_backend_owner_isolation", PHASE1R_STACK_DONE },
    { "scene_backend_manual_interpolation_contract", PHASE1R_STACK_DONE },
    { "hook_prototype_dry_run_runtime_gates", PHASE1R_STACK_DONE },
    { "first_real_fo4_callback_candidate_surface", PHASE1R_STACK_DONE },
    { "first_real_in_game_proxy_actor_candidate_surface", PHASE1R_STACK_DONE },
    { "first_live_fo4_callback_validation", PHASE1R_STACK_DONE },
    { "first_live_in_game_proxy_validation", PHASE1R_STACK_DONE },
    { "first_in_engine_goal0_movement_validation_harness", PHASE1R_STACK_DONE },
    { "first_in_engine_goal0_movement_validation", PHASE1R_STACK_DONE },
    { "real_hook_evidence_collection", PHASE1R_STACK_DONE },
    { "real_hook_arming_gate", PHASE1R_STACK_DONE },
    { "real_hook_fallback_plan", PHASE1R_STACK_DONE },
    { "real_hook_wire_plan", PHASE1R_STACK_DONE },
    { "real_hook_wire_readiness_bundle", PHASE1R_STACK_DONE },
    { "first_real_local_hook_wiring", PHASE1R_STACK_DONE },
    { "first_real_local_hook_dry_run_in_fo4", PHASE1R_STACK_DONE },
    { "first_real_local_hook_armed_attempt_in_fo4", PHASE1R_STACK_CURRENT },
    { "first_real_plugin_dll_drop_in_f4se", PHASE1R_STACK_NEXT }
};

size_t phase1r_stack_tracker_snapshot(Phase1RStackEntry* out_entries, size_t capacity) {
    size_t i;
    size_t count = sizeof(g_entries) / sizeof(g_entries[0]);
    if (!out_entries || capacity == 0) return count;
    if (capacity > count) capacity = count;
    for (i = 0; i < capacity; ++i) out_entries[i] = g_entries[i];
    return count;
}

const char* phase1r_stack_status_name(Phase1RStackStatus status) {
    switch (status) {
        case PHASE1R_STACK_DONE: return "done";
        case PHASE1R_STACK_CURRENT: return "current";
        case PHASE1R_STACK_NEXT: return "next";
        case PHASE1R_STACK_BLOCKED: return "blocked";
        default: return "unknown";
    }
}
