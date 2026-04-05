#include <assert.h>
#include <string.h>
#include "dialogue_selection_policy.h"

int main(void) {
    SessionState s;
    MsgDialogueSelect sel;
    DialogueSelectionResult r;

    memset(&s, 0, sizeof(s));
    s.dialogue.active = true;
    s.dialogue.dialogue_id = 11;
    s.dialogue.choice_count = 2;
    s.dialogue.choices[0].choice_id = 100;
    s.dialogue.choices[0].enabled = 1;
    s.dialogue.choices[1].choice_id = 101;
    s.dialogue.choices[1].enabled = 0;

    memset(&sel, 0, sizeof(sel));
    sel.dialogue_id = 11;
    sel.choice_id = 100;
    assert(dsp_apply_selection(&s, 2, &sel, &r));
    assert(r == DSEL_ACCEPTED);
    assert(s.dialogue.last_selected_choice_id == 100);
    assert(s.dialogue.last_selected_by_player == 2);

    sel.choice_id = 101;
    assert(!dsp_apply_selection(&s, 2, &sel, &r));
    assert(r == DSEL_REJECT_CHOICE_DISABLED);

    sel.dialogue_id = 99;
    sel.choice_id = 100;
    assert(!dsp_apply_selection(&s, 2, &sel, &r));
    assert(r == DSEL_REJECT_DIALOGUE_MISMATCH);

    return 0;
}
