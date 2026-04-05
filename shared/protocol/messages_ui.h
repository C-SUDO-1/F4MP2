#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgDialogueOpen {
    DialogueId dialogue_id;
    ActorNetId npc_actor_net_id;
    ActorNetId speaker_actor_net_id;
} MsgDialogueOpen;

typedef struct MsgDialogueLine {
    DialogueId dialogue_id;
    ActorNetId speaker_actor_net_id;
} MsgDialogueLine;

typedef struct MsgDialogueChoicesHeader {
    DialogueId dialogue_id;
    uint16_t   choice_count;
    uint16_t   reserved0;
} MsgDialogueChoicesHeader;

typedef struct MsgDialogueChoiceRecord {
    uint32_t choice_id;
    uint8_t  enabled;
    uint8_t  reserved0[3];
} MsgDialogueChoiceRecord;

typedef struct MsgDialogueSelect {
    DialogueId dialogue_id;
    uint32_t   choice_id;
} MsgDialogueSelect;

typedef struct MsgDialogueClose {
    DialogueId dialogue_id;
    uint8_t    reason;
    uint8_t    reserved0[3];
} MsgDialogueClose;

typedef struct MsgQuestUpdateHeader {
    QuestId   quest_id;
    uint16_t  stage;
    uint8_t   tracked;
    uint8_t   objective_count;
} MsgQuestUpdateHeader;

typedef struct MsgQuestObjectiveRecord {
    ObjectiveId objective_id;
    uint8_t     state;
    uint8_t     reserved0[3];
} MsgQuestObjectiveRecord;
#pragma pack(pop)

enum DialogueCloseReason { DLG_CLOSE_ENDED=0, DLG_CLOSE_CANCELLED=1, DLG_CLOSE_INTERRUPTED=2 };
enum QuestObjectiveState { QOBJ_ACTIVE=0, QOBJ_COMPLETED=1, QOBJ_FAILED=2 };

