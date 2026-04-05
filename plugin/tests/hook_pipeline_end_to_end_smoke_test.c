#include <assert.h>
#include <string.h>
#include "live_hook_pipeline_stub.h"
#include "proxy_runtime.h"

int main(void) {
    PlayerHostEvent pe;
    WorkshopHostEvent we;
    ActorHostEvent ae;
    DialogueHostLineEvent de;
    QuestHostObjectiveEvent qe;
    HookDispatchStats stats;
    const ProxyPlayerRecord* player;
    const ProxyObjectRecord* object;
    const ProxyActorRecord* actor;
    const ProxyDialogueState* dialogue;
    const ProxyQuestState* quest;

    proxy_runtime_init();
    lhps_init();

    memset(&pe, 0, sizeof(pe));
    pe.player_id = 1;
    pe.position.x = 10.0f;
    pe.rotation.yaw = 90.0f;
    pe.velocity.x = 1.0f;
    pe.stance = STANCE_RUN;
    pe.anim_state = 7;
    pe.equipped_form_id = 0x100;
    assert(lhps_submit_player_event(&pe));
    player = proxy_runtime_get_player(1);
    assert(player && player->position.x == 10.0f && player->equipped_form_id == 0x100);

    memset(&we, 0, sizeof(we));
    we.kind = WHE_PLACE;
    we.object_net_id = 500;
    we.form_id = 0xAAAAAAA1u;
    we.position.z = 2.0f;
    assert(lhps_submit_workshop_event(&we));
    object = proxy_runtime_get_object(500);
    assert(object && object->form_id == 0xAAAAAAA1u);

    memset(&ae, 0, sizeof(ae));
    ae.kind = AHE_SPAWN;
    ae.actor_net_id = 700;
    ae.base_form_id = 0xDEAD;
    ae.position.y = 3.0f;
    ae.actor_kind = ACTOR_KIND_NPC;
    assert(lhps_submit_actor_event(&ae));
    actor = proxy_runtime_get_actor(700);
    assert(actor && actor->base_form_id == 0xDEAD);

    memset(&de, 0, sizeof(de));
    de.dialogue_id = 42;
    de.npc_actor_net_id = 700;
    de.speaker_actor_net_id = 700;
    assert(lhps_submit_dialogue_event(&de));
    memset(&de, 0, sizeof(de));
    de.dialogue_id = 42;
    de.speaker_actor_net_id = 700;
    de.text = "Hello there";
    assert(lhps_submit_dialogue_event(&de));
    dialogue = proxy_runtime_get_dialogue();
    assert(dialogue && dialogue->active && dialogue->dialogue_id == 42);
    assert(strncmp(dialogue->current_line, "Hello there", 11) == 0);

    memset(&qe, 0, sizeof(qe));
    qe.quest_id = 77;
    qe.stage = 10;
    qe.tracked = 1;
    qe.objective_count = 1;
    qe.objectives[0].objective_id = 9001;
    qe.objectives[0].state = QOBJ_ACTIVE;
    qe.objective_texts[0] = "Go there";
    assert(lhps_submit_quest_event(&qe));
    quest = proxy_runtime_get_quest();
    assert(quest && quest->loaded && quest->quest_id == 77 && quest->objective_count == 1);

    stats = lhps_get_stats();
    assert(stats.player_events == 1);
    assert(stats.workshop_events == 1);
    assert(stats.actor_events == 1);
    assert(stats.dialogue_events == 2);
    assert(stats.quest_events == 1);
    assert(stats.failures == 0);
    return 0;
}
