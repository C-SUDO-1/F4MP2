#include <assert.h>
#include <string.h>
#include "transport_loop.h"
#include "host_event_mirror.h"
#include "service_state_models.h"
#include "session_rules.h"

int main(void) {
    SessionState s; TransportLoopContext ctx; MsgDialogueChoiceRecord choices[2]; MsgQuestObjectiveRecord objs[1]; const char* dtexts[2] = {"A","B"}; const char* qtexts[1] = {"Do thing"};
    memset(&s,0,sizeof(s)); s.in_use=true; s.session_id=1; s.tick_rate=20; s.rules = RULE_QUEST_MIRROR_ENABLED; s.players[0].connected=true; s.players[0].player_id=1; s.players[0].connection_id=111;
    tl_init(&ctx,&s);
    assert(hem_dialogue_open(&ctx, 10, 20, 20));
    assert(s.dialogue.active);
    assert(hem_dialogue_line(&ctx, 10, 20, "Hello"));
    assert(strcmp(s.dialogue.current_line, "Hello") == 0);
    memset(choices,0,sizeof(choices)); choices[0].choice_id=1; choices[0].enabled=1; choices[1].choice_id=2; choices[1].enabled=1;
    assert(hem_dialogue_choices(&ctx, 10, choices, dtexts, 2));
    assert(hem_dialogue_close(&ctx, 10, DLG_CLOSE_ENDED));
    assert(!s.dialogue.active);
    memset(objs,0,sizeof(objs)); objs[0].objective_id=1; objs[0].state=QOBJ_ACTIVE;
    assert(hem_quest_update(&ctx, 7, 2, 1, objs, qtexts, 1));
    assert(s.quest.loaded);
    assert(s.quest.quest_id == 7);
    assert(strcmp(s.quest.objectives[0].text, "Do thing") == 0);
    assert(tl_get_output_count(&ctx) == 5);
    return 0;
}
