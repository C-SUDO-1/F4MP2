#pragma once

#include <stdbool.h>
#include "messages_ui.h"

void dqhs_init(void);
bool dqhs_ingest_dialogue_open(const MsgDialogueOpen* msg);
bool dqhs_ingest_dialogue_line(const MsgDialogueLine* msg, const char* text);
bool dqhs_ingest_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]);
bool dqhs_ingest_dialogue_close(const MsgDialogueClose* msg);
bool dqhs_ingest_quest_event(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]);
