#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_ui.h"

typedef struct DialogueHostLineEvent {
    DialogueId dialogue_id;
    ActorNetId npc_actor_net_id;
    ActorNetId speaker_actor_net_id;
    const char* text;
} DialogueHostLineEvent;

typedef struct QuestHostObjectiveEvent {
    QuestId quest_id;
    uint16_t stage;
    uint8_t tracked;
    uint8_t objective_count;
    MsgQuestObjectiveRecord objectives[4];
    const char* objective_texts[4];
} QuestHostObjectiveEvent;

bool dqet_translate_open(const DialogueHostLineEvent* ev, MsgDialogueOpen* out_msg);
bool dqet_translate_line(const DialogueHostLineEvent* ev, MsgDialogueLine* out_msg, const char** out_text);
bool dqet_translate_quest(const QuestHostObjectiveEvent* ev, MsgQuestUpdateHeader* out_hdr, MsgQuestObjectiveRecord* out_objs, const char** out_texts, uint8_t max_objs);
