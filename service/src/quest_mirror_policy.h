#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "service_state_models.h"
#include "messages_ui.h"

typedef struct QuestMirrorPolicyResult {
    bool accepted;
    uint8_t reason_code;
} QuestMirrorPolicyResult;

bool qmp_validate_update(
    SessionState* s,
    const MsgQuestUpdateHeader* h,
    const MsgQuestObjectiveRecord* objs,
    QuestMirrorPolicyResult* out_result
);
