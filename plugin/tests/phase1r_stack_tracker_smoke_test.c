#include <assert.h>
#include <string.h>

#include "phase1r_stack_tracker.h"

int main(void) {
    Phase1RStackEntry entries[20];
    size_t count = phase1r_stack_tracker_snapshot(entries, 20);
    assert(count == 20);
    assert(strcmp(entries[0].name, "goal0_harness_join_local_id") == 0);
    assert(entries[0].status == PHASE1R_STACK_DONE);
    assert(entries[4].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[5].name, "first_real_fo4_callback_candidate_surface") == 0);
    assert(entries[5].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[6].name, "first_real_in_game_proxy_actor_candidate_surface") == 0);
    assert(entries[6].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[7].name, "first_live_fo4_callback_validation") == 0);
    assert(entries[7].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[8].name, "first_live_in_game_proxy_validation") == 0);
    assert(entries[8].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[9].name, "first_in_engine_goal0_movement_validation_harness") == 0);
    assert(entries[9].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[10].name, "first_in_engine_goal0_movement_validation") == 0);
    assert(entries[10].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[11].name, "real_hook_evidence_collection") == 0);
    assert(entries[11].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[12].name, "real_hook_arming_gate") == 0);
    assert(entries[12].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[13].name, "real_hook_fallback_plan") == 0);
    assert(entries[13].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[14].name, "real_hook_wire_plan") == 0);
    assert(entries[14].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[15].name, "real_hook_wire_readiness_bundle") == 0);
    assert(entries[15].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[16].name, "first_real_local_hook_wiring") == 0);
    assert(entries[16].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[17].name, "first_real_local_hook_dry_run_in_fo4") == 0);
    assert(entries[17].status == PHASE1R_STACK_DONE);
    assert(strcmp(entries[18].name, "first_real_local_hook_armed_attempt_in_fo4") == 0);
    assert(entries[18].status == PHASE1R_STACK_CURRENT);
    assert(strcmp(phase1r_stack_status_name(entries[18].status), "current") == 0);
    assert(strcmp(entries[19].name, "first_real_plugin_dll_drop_in_f4se") == 0);
    assert(entries[19].status == PHASE1R_STACK_NEXT);
    return 0;
}
