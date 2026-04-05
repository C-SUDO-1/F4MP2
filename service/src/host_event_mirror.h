#pragma once

#include <stdbool.h>
#include "transport_loop.h"
#include "messages_ui.h"

bool hem_dialogue_open(TransportLoopContext* ctx, DialogueId dialogue_id, ActorNetId npc_actor_net_id, ActorNetId speaker_actor_net_id);
bool hem_dialogue_line(TransportLoopContext* ctx, DialogueId dialogue_id, ActorNetId speaker_actor_net_id, const char* text);
bool hem_dialogue_choices(TransportLoopContext* ctx, DialogueId dialogue_id, const MsgDialogueChoiceRecord* choices, const char* const* texts, uint16_t choice_count);
bool hem_dialogue_close(TransportLoopContext* ctx, DialogueId dialogue_id, uint8_t reason);
bool hem_quest_update(TransportLoopContext* ctx, QuestId quest_id, uint16_t stage, uint8_t tracked, const MsgQuestObjectiveRecord* objs, const char* const* texts, uint8_t objective_count);

bool hem_try_pop_dialogue_select(TransportLoopContext* ctx, PlayerId* out_player_id, MsgDialogueSelect* out_select);

bool hem_apply_guest_dialogue_select(TransportLoopContext* ctx, PlayerId player_id, const MsgDialogueSelect* select);
