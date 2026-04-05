#include "dialogue_quest_hook_shim.h"
#include "fo4_bridge_ingress.h"

void dqhs_init(void) {}

bool dqhs_ingest_dialogue_open(const MsgDialogueOpen* msg) { return fbi_ingest_dialogue_open(msg); }
bool dqhs_ingest_dialogue_line(const MsgDialogueLine* msg, const char* text) { return fbi_ingest_dialogue_line(msg, text); }
bool dqhs_ingest_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]) { return fbi_ingest_dialogue_choices(h, choices, texts); }
bool dqhs_ingest_dialogue_close(const MsgDialogueClose* msg) { return fbi_ingest_dialogue_close(msg); }
bool dqhs_ingest_quest_event(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]) { return fbi_ingest_quest_update(h, objs, texts); }
