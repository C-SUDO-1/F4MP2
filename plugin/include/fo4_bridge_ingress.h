#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_player.h"
#include "messages_actor.h"
#include "messages_workshop.h"
#include "messages_ui.h"
#include "messages_combat.h"

void fbi_init(void);

bool fbi_ingest_remote_player_state(const MsgPlayerState* msg);
bool fbi_ingest_actor_spawn(const MsgActorSpawn* msg);
bool fbi_ingest_actor_state(const MsgActorState* msg);
bool fbi_ingest_actor_despawn(const MsgActorDespawn* msg);

bool fbi_ingest_object_spawn(const MsgObjectSpawn* msg);
bool fbi_ingest_object_update(const MsgObjectUpdate* msg);
bool fbi_ingest_object_despawn(const MsgObjectDespawn* msg);

bool fbi_ingest_damage_result(const MsgDamageResult* msg);
bool fbi_ingest_death_event(const MsgDeathEvent* msg);

bool fbi_ingest_dialogue_open(const MsgDialogueOpen* msg);
bool fbi_ingest_dialogue_line(const MsgDialogueLine* msg, const char* text);
bool fbi_ingest_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]);
bool fbi_ingest_dialogue_close(const MsgDialogueClose* msg);
bool fbi_ingest_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]);

bool fbi_get_last_player_state(MsgPlayerState* out);
