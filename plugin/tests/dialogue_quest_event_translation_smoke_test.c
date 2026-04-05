#include <assert.h>
#include <string.h>
#include "dialogue_quest_event_translation.h"

int main(void) {
    DialogueHostLineEvent dev;
    MsgDialogueOpen dopen;
    MsgDialogueLine dline;
    const char* text = 0;
    QuestHostObjectiveEvent qev;
    MsgQuestUpdateHeader qh;
    MsgQuestObjectiveRecord objs[4];
    const char* texts[4];
    memset(&dev, 0, sizeof(dev));
    memset(&dopen, 0, sizeof(dopen));
    memset(&dline, 0, sizeof(dline));
    dev.dialogue_id = 10;
    dev.npc_actor_net_id = 20;
    dev.speaker_actor_net_id = 21;
    dev.text = "Hello";
    assert(dqet_translate_open(&dev, &dopen));
    assert(dopen.dialogue_id == 10 && dopen.npc_actor_net_id == 20 && dopen.speaker_actor_net_id == 21);
    assert(dqet_translate_line(&dev, &dline, &text));
    assert(dline.dialogue_id == 10 && strcmp(text, "Hello") == 0);

    memset(&qev, 0, sizeof(qev));
    qev.quest_id = 99;
    qev.stage = 5;
    qev.tracked = 1;
    qev.objective_count = 2;
    qev.objectives[0].objective_id = 1; qev.objectives[0].state = QOBJ_ACTIVE; qev.objective_texts[0] = "Find item";
    qev.objectives[1].objective_id = 2; qev.objectives[1].state = QOBJ_COMPLETED; qev.objective_texts[1] = "Leave vault";
    assert(dqet_translate_quest(&qev, &qh, objs, texts, 4));
    assert(qh.quest_id == 99 && qh.objective_count == 2);
    assert(objs[1].objective_id == 2 && strcmp(texts[1], "Leave vault") == 0);
    return 0;
}
