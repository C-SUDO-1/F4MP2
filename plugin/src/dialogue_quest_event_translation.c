#include "dialogue_quest_event_translation.h"
#include <string.h>

bool dqet_translate_open(const DialogueHostLineEvent* ev, MsgDialogueOpen* out_msg) {
    if (!ev || !out_msg) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->dialogue_id = ev->dialogue_id;
    out_msg->npc_actor_net_id = ev->npc_actor_net_id;
    out_msg->speaker_actor_net_id = ev->speaker_actor_net_id;
    return true;
}

bool dqet_translate_line(const DialogueHostLineEvent* ev, MsgDialogueLine* out_msg, const char** out_text) {
    if (!ev || !out_msg || !out_text) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->dialogue_id = ev->dialogue_id;
    out_msg->speaker_actor_net_id = ev->speaker_actor_net_id;
    *out_text = ev->text ? ev->text : "";
    return true;
}

bool dqet_translate_quest(const QuestHostObjectiveEvent* ev, MsgQuestUpdateHeader* out_hdr, MsgQuestObjectiveRecord* out_objs, const char** out_texts, uint8_t max_objs) {
    uint8_t i, count;
    if (!ev || !out_hdr || !out_objs || !out_texts) return false;
    memset(out_hdr, 0, sizeof(*out_hdr));
    out_hdr->quest_id = ev->quest_id;
    out_hdr->stage = ev->stage;
    out_hdr->tracked = ev->tracked;
    count = ev->objective_count < max_objs ? ev->objective_count : max_objs;
    out_hdr->objective_count = count;
    for (i = 0; i < count; ++i) {
        out_objs[i] = ev->objectives[i];
        out_texts[i] = ev->objective_texts[i] ? ev->objective_texts[i] : "";
    }
    return true;
}
