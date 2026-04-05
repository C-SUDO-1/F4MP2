#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "player_hook_shim.h"
#include "actor_hook_shim.h"
#include "workshop_hook_shim.h"
#include "dialogue_quest_hook_shim.h"
#include "proxy_runtime.h"

int main(void) {
    MsgPlayerState ps = {0};
    MsgActorSpawn as = {0};
    MsgActorState ast = {0};
    MsgObjectSpawn os = {0};
    MsgObjectUpdate ou = {0};
    MsgDialogueOpen dopen = {0};
    MsgDialogueLine dline = {0};
    MsgDialogueChoicesHeader dh = {0};
    MsgDialogueChoiceRecord choice = {0};
    MsgQuestUpdateHeader qh = {0};
    MsgQuestObjectiveRecord qo = {0};
    char dtexts[1][256] = {{0}};
    char qtexts[1][256] = {{0}};

    proxy_runtime_init();
    phs_init(); ahs_init(); whs_init(); dqhs_init();

    ps.player_id = 7; ps.position.x = 10.0f;
    assert(phs_ingest_player_snapshot(&ps));
    MsgPlayerState out = {0};
    assert(phs_get_last_player_snapshot(&out));
    assert(out.player_id == 7);

    as.actor_net_id = 100; as.position.x = 1.0f;
    assert(ahs_ingest_actor_spawn(&as));
    ast.actor_net_id = 100; ast.health_norm = 0.75f;
    assert(ahs_ingest_actor_snapshot(&ast));

    os.object_net_id = 200; os.form_id = 0xAAAAAAAAu;
    assert(whs_ingest_workshop_spawn(&os));
    ou.object_net_id = 200; ou.fields_mask = OUF_STATE_FLAGS;
    assert(whs_ingest_workshop_event(&ou));

    dopen.dialogue_id = 300; dopen.npc_actor_net_id = 100;
    assert(dqhs_ingest_dialogue_open(&dopen));
    dline.dialogue_id = 300;
    assert(dqhs_ingest_dialogue_line(&dline, "hello"));
    dh.dialogue_id = 300; dh.choice_count = 1; choice.choice_id = 1; choice.enabled = 1; strcpy(dtexts[0], "Yes");
    assert(dqhs_ingest_dialogue_choices(&dh, &choice, dtexts));

    qh.quest_id = 400; qh.objective_count = 1; qo.objective_id = 10; qo.state = QOBJ_ACTIVE; strcpy(qtexts[0], "Go there");
    assert(dqhs_ingest_quest_event(&qh, &qo, qtexts));

    puts("hook_shim_smoke_test: ok");
    return 0;
}
