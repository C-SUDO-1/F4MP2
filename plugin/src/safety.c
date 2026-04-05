#include "plugin_safety_contract.h"

static SafetyEvaluation mk(SafetyAction action, uint16_t reason_code) {
    SafetyEvaluation s; s.action = action; s.reason_code = reason_code; return s;
}

SafetyEvaluation safety_can_guest_mutate_world(PlayerId player_id) {
    (void)player_id;
    return mk(SAFETY_HOST_ONLY, 100);
}

SafetyEvaluation safety_can_guest_start_dialogue(PlayerId player_id) {
    (void)player_id;
    return mk(SAFETY_HOST_ONLY, 101);
}

SafetyEvaluation safety_can_guest_trigger_quest_change(PlayerId player_id) {
    (void)player_id;
    return mk(SAFETY_BLOCK, 102);
}

SafetyEvaluation safety_can_guest_place_object(PlayerId player_id, uint32_t form_id) {
    (void)player_id;
    (void)form_id;
    return mk(SAFETY_ALLOW, 0);
}
