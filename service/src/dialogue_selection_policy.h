#pragma once

#include <stdbool.h>
#include "service_state_models.h"
#include "messages_ui.h"

typedef enum DialogueSelectionResult {
    DSEL_ACCEPTED = 0,
    DSEL_REJECT_NO_ACTIVE_DIALOGUE = 1,
    DSEL_REJECT_DIALOGUE_MISMATCH = 2,
    DSEL_REJECT_CHOICE_NOT_FOUND = 3,
    DSEL_REJECT_CHOICE_DISABLED = 4
} DialogueSelectionResult;

DialogueSelectionResult dsp_validate_selection(
    const SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select
);

bool dsp_apply_selection(
    SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select,
    DialogueSelectionResult* out_result
);
