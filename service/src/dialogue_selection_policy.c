#include "dialogue_selection_policy.h"

DialogueSelectionResult dsp_validate_selection(
    const SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select
) {
    uint16_t i;
    (void)player_id;
    if (!s || !select) return DSEL_REJECT_NO_ACTIVE_DIALOGUE;
    if (!s->dialogue.active) return DSEL_REJECT_NO_ACTIVE_DIALOGUE;
    if (s->dialogue.dialogue_id != select->dialogue_id) return DSEL_REJECT_DIALOGUE_MISMATCH;
    for (i = 0; i < s->dialogue.choice_count; ++i) {
        if (s->dialogue.choices[i].choice_id == select->choice_id) {
            return s->dialogue.choices[i].enabled ? DSEL_ACCEPTED : DSEL_REJECT_CHOICE_DISABLED;
        }
    }
    return DSEL_REJECT_CHOICE_NOT_FOUND;
}

bool dsp_apply_selection(
    SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select,
    DialogueSelectionResult* out_result
) {
    DialogueSelectionResult r = dsp_validate_selection(s, player_id, select);
    if (out_result) *out_result = r;
    if (r != DSEL_ACCEPTED) return false;
    s->dialogue.last_selected_choice_id = select->choice_id;
    s->dialogue.last_selected_by_player = player_id;
    return true;
}
