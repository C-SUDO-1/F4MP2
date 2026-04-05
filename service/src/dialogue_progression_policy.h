#pragma once

#include <stdbool.h>
#include "service_state_models.h"
#include "messages_ui.h"

typedef enum DialogueProgressionResult {
    DPR_REJECTED = 0,
    DPR_ADVANCED_LINE = 1,
    DPR_CLOSE_DIALOGUE = 2
} DialogueProgressionResult;

typedef struct DialogueProgressionOutput {
    DialogueProgressionResult result;
    MsgDialogueLine line;
    char line_text[256];
    MsgDialogueClose close;
} DialogueProgressionOutput;

bool dpp_apply_selection(
    SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select,
    DialogueProgressionOutput* out
);
