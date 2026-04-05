#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "messages_player.h"
#include "messages_actor.h"
#include "messages_workshop.h"
#include "messages_control.h"
#include "messages_combat.h"
#include "messages_ui.h"

bool plugin_apply_packet(const uint8_t* bytes, size_t length);

bool pa_apply_player_state(const MsgPlayerState* msg);
bool pa_apply_actor_spawn(const MsgActorSpawn* msg);
bool pa_apply_actor_state(const MsgActorState* msg);
bool pa_apply_actor_despawn(const MsgActorDespawn* msg);
bool pa_apply_object_spawn(const MsgObjectSpawn* msg);
bool pa_apply_object_update(const MsgObjectUpdate* msg);
bool pa_apply_object_despawn(const MsgObjectDespawn* msg);
bool pa_apply_damage_result(const MsgDamageResult* msg);
bool pa_apply_death_event(const MsgDeathEvent* msg);
bool pa_apply_dialogue_open(const MsgDialogueOpen* msg);
bool pa_apply_dialogue_line(const MsgDialogueLine* msg, const char* text);
bool pa_apply_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]);
bool pa_apply_dialogue_close(const MsgDialogueClose* msg);
bool pa_apply_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]);
