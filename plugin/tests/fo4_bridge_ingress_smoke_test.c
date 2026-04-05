#include <assert.h>
#include <string.h>
#include "fo4_bridge_ingress.h"
#include "engine_bridge_stub.h"

int main(void) {
    MsgPlayerState ps;
    MsgDialogueOpen dopen;
    MsgDialogueLine dline;
    MsgDialogueChoicesHeader dh;
    MsgDialogueChoiceRecord choices[1];
    MsgQuestUpdateHeader qh;
    MsgQuestObjectiveRecord qo[1];
    MsgPlayerState out;
    char texts[1][256];

    ebstub_init();
    fbi_init();

    memset(&ps, 0, sizeof(ps));
    ps.player_id = 2;
    ps.position.x = 10.0f;
    ps.position.y = 20.0f;
    ps.position.z = 30.0f;
    assert(fbi_ingest_remote_player_state(&ps));
    assert(fbi_get_last_player_state(&out));
    assert(out.player_id == 2);
    assert(ebstub_player_count() == 1);

    memset(&dopen, 0, sizeof(dopen));
    dopen.dialogue_id = 7;
    dopen.npc_actor_net_id = 100;
    dopen.speaker_actor_net_id = 100;
    assert(fbi_ingest_dialogue_open(&dopen));

    memset(&dline, 0, sizeof(dline));
    dline.dialogue_id = 7;
    dline.speaker_actor_net_id = 100;
    assert(fbi_ingest_dialogue_line(&dline, "hello"));

    memset(&dh, 0, sizeof(dh));
    dh.dialogue_id = 7;
    dh.choice_count = 1;
    memset(&choices[0], 0, sizeof(choices[0]));
    choices[0].choice_id = 42;
    choices[0].enabled = 1;
    strncpy(texts[0], "yes", sizeof(texts[0]) - 1);
    assert(fbi_ingest_dialogue_choices(&dh, choices, texts));

    memset(&qh, 0, sizeof(qh));
    qh.quest_id = 9;
    qh.stage = 20;
    qh.tracked = 1;
    qh.objective_count = 1;
    memset(&qo[0], 0, sizeof(qo[0]));
    qo[0].objective_id = 1;
    qo[0].state = 0;
    memset(texts[0], 0, sizeof(texts[0]));
    strncpy(texts[0], "reach target", sizeof(texts[0]) - 1);
    assert(fbi_ingest_quest_update(&qh, qo, texts));

    assert(ebstub_has_active_dialogue());
    assert(ebstub_get_quest()->loaded);
    return 0;
}
