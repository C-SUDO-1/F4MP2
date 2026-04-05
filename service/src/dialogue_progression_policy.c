#include "dialogue_progression_policy.h"
#include "dialogue_selection_policy.h"
#include <stdio.h>
#include <string.h>

bool dpp_apply_selection(
    SessionState* s,
    PlayerId player_id,
    const MsgDialogueSelect* select,
    DialogueProgressionOutput* out
) {
    DialogueSelectionResult sr;
    uint16_t i;
    const char* choice_text = NULL;

    if (!s || !select || !out) return false;
    memset(out, 0, sizeof(*out));

    if (!dsp_apply_selection(s, player_id, select, &sr)) {
        out->result = DPR_REJECTED;
        return false;
    }

    for (i = 0; i < s->dialogue.choice_count; ++i) {
        if (s->dialogue.choices[i].choice_id == select->choice_id) {
            choice_text = s->dialogue.choices[i].text;
            break;
        }
    }

    if (!choice_text) {
        out->result = DPR_REJECTED;
        return false;
    }

    /* Conservative vanilla-first mirror policy:
       surface the selected choice as a mirrored host-side acknowledgement,
       then close the dialogue until real host hook progression is wired. */
    out->result = DPR_ADVANCED_LINE;
    out->line.dialogue_id = s->dialogue.dialogue_id;
    out->line.speaker_actor_net_id = s->dialogue.speaker_actor_net_id;
    (void)snprintf(out->line_text, sizeof(out->line_text), "Selected: %s", choice_text);

    strncpy(s->dialogue.current_line, out->line_text, sizeof(s->dialogue.current_line) - 1);
    s->dialogue.choice_count = 0;

    out->close.dialogue_id = s->dialogue.dialogue_id;
    out->close.reason = DLG_CLOSE_ENDED;

    /* caller may choose to also broadcast close immediately */
    return true;
}
