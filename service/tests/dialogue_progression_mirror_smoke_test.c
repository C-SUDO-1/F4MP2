#include <assert.h>
#include <string.h>
#include "host_event_mirror.h"

int main(void) {
    SessionState session;
    TransportLoopContext ctx;
    MsgDialogueChoiceRecord choice;
    const char* texts[1] = {"Proceed"};
    MsgDialogueSelect sel;

    memset(&session, 0, sizeof(session));
    session.in_use = true;
    session.session_id = 1;
    session.connected_count = 1;
    session.players[0].connected = true;
    session.players[0].player_id = 1;
    session.players[0].connection_id = 11;

    tl_init(&ctx, &session);
    assert(hem_dialogue_open(&ctx, 5, 100, 100));
    memset(&choice, 0, sizeof(choice));
    choice.choice_id = 44;
    choice.enabled = 1;
    assert(hem_dialogue_choices(&ctx, 5, &choice, texts, 1));

    memset(&sel, 0, sizeof(sel));
    sel.dialogue_id = 5;
    sel.choice_id = 44;
    assert(hem_apply_guest_dialogue_select(&ctx, 1, &sel));
    assert(session.dialogue.active == false);
    assert(session.dialogue.last_selected_choice_id == 44);
    assert(tl_get_output_count(&ctx) >= 4);
    return 0;
}
