#include "quest_mirror_policy.h"
#include <string.h>

static bool objective_state_valid(uint8_t state) {
    return state == QOBJ_ACTIVE || state == QOBJ_COMPLETED || state == QOBJ_FAILED;
}

static bool objective_transition_valid(uint8_t old_state, uint8_t new_state) {
    if (old_state == new_state) return true;
    if (old_state == QOBJ_ACTIVE && (new_state == QOBJ_COMPLETED || new_state == QOBJ_FAILED)) return true;
    return false;
}

bool qmp_validate_update(
    SessionState* s,
    const MsgQuestUpdateHeader* h,
    const MsgQuestObjectiveRecord* objs,
    QuestMirrorPolicyResult* out_result
) {
    uint8_t i;
    QuestMirrorPolicyResult local = { false, 0 };
    if (!out_result) out_result = &local;
    out_result->accepted = false;
    out_result->reason_code = 1;
    if (!s || !h) return false;
    if (!(s->rules & RULE_QUEST_MIRROR_ENABLED)) {
        out_result->reason_code = 2;
        return false;
    }
    if (h->objective_count > F4MP_MAX_QUEST_OBJECTIVES) {
        out_result->reason_code = 3;
        return false;
    }
    if (s->quest.loaded && s->quest.quest_id != 0 && s->quest.quest_id != h->quest_id) {
        /* vanilla-first scaffold tracks one mirrored quest at a time */
        out_result->reason_code = 4;
        return false;
    }
    if (s->quest.loaded && s->quest.quest_id == h->quest_id && h->stage < s->quest.stage) {
        out_result->reason_code = 6;
        return false;
    }
    for (i = 0; i < h->objective_count; ++i) {
        if (!objective_state_valid(objs[i].state)) {
            out_result->reason_code = 5;
            return false;
        }
        if (s->quest.loaded && s->quest.quest_id == h->quest_id) {
            uint8_t j;
            for (j = 0; j < s->quest.objective_count; ++j) {
                if (s->quest.objectives[j].objective_id == objs[i].objective_id) {
                    if (!objective_transition_valid(s->quest.objectives[j].state, objs[i].state)) {
                        out_result->reason_code = 7;
                        return false;
                    }
                    break;
                }
            }
        }
    }
    out_result->accepted = true;
    out_result->reason_code = 0;
    return true;
}
