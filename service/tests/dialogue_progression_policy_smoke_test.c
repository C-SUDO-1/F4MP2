#include <assert.h>
#include <string.h>
#include "dialogue_progression_policy.h"

int main(void) {
    SessionState s;
    MsgDialogueSelect sel;
    DialogueProgressionOutput out;

    memset(&s, 0, sizeof(s));
    s.dialogue.active = true;
    s.dialogue.dialogue_id = 77;
    s.dialogue.speaker_actor_net_id = 300;
    s.dialogue.choice_count = 1;
    s.dialogue.choices[0].choice_id = 99;
    s.dialogue.choices[0].enabled = 1;
    strcpy(s.dialogue.choices[0].text, "Yes");

    memset(&sel, 0, sizeof(sel));
    sel.dialogue_id = 77;
    sel.choice_id = 99;

    assert(dpp_apply_selection(&s, 1, &sel, &out));
    assert(out.result == DPR_ADVANCED_LINE);
    assert(strstr(out.line_text, "Yes") != NULL);
    assert(s.dialogue.choice_count == 0);
    assert(out.close.dialogue_id == 77);
    return 0;
}
