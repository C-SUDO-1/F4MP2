#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "packet_writer.h"
#include "packet_reader.h"
#include "packet_header.h"
#include "messages_control.h"
#include "messages_player.h"
#include "messages_actor.h"
#include "messages_combat.h"
#include "messages_workshop.h"
#include "messages_ui.h"

bool encode_packet_header(PacketWriter* w, const PacketHeader* h);
bool decode_packet_header(PacketReader* r, PacketHeader* h);
bool encode_message_header(PacketWriter* w, const MessageHeader* h);
bool decode_message_header(PacketReader* r, MessageHeader* h);

bool encode_msg_hello(PacketWriter* w, const MsgHello* m, const char* player_guid, const char* player_name);
bool decode_msg_hello(PacketReader* r, MsgHello* m, char* player_guid, size_t player_guid_cap, char* player_name, size_t player_name_cap);
bool encode_msg_welcome(PacketWriter* w, const MsgWelcome* m);
bool decode_msg_welcome(PacketReader* r, MsgWelcome* m);
bool encode_msg_ping(PacketWriter* w, const MsgPing* m);
bool decode_msg_ping(PacketReader* r, MsgPing* m);
bool encode_msg_pong(PacketWriter* w, const MsgPong* m);
bool decode_msg_pong(PacketReader* r, MsgPong* m);
bool encode_msg_disconnect(PacketWriter* w, const MsgDisconnect* m);
bool decode_msg_disconnect(PacketReader* r, MsgDisconnect* m);
bool encode_msg_profile_snapshot(PacketWriter* w, const MsgProfileSnapshot* m);
bool decode_msg_profile_snapshot(PacketReader* r, MsgProfileSnapshot* m);
bool encode_msg_profile_items(PacketWriter* w, const MsgProfileItemsHeader* h, const MsgProfileItemRecord* items);
bool decode_msg_profile_items(PacketReader* r, MsgProfileItemsHeader* h, MsgProfileItemRecord* items_out, uint16_t max_items);
bool encode_msg_profile_perks(PacketWriter* w, const MsgProfilePerksHeader* h, const MsgProfilePerkRecord* perks);
bool decode_msg_profile_perks(PacketReader* r, MsgProfilePerksHeader* h, MsgProfilePerkRecord* perks_out, uint16_t max_perks);
bool encode_msg_profile_appearance(PacketWriter* w, const MsgProfileAppearance* m);
bool decode_msg_profile_appearance(PacketReader* r, MsgProfileAppearance* m);
bool encode_msg_input_state(PacketWriter* w, const MsgInputState* m);
bool decode_msg_input_state(PacketReader* r, MsgInputState* m);
bool encode_msg_player_state(PacketWriter* w, const MsgPlayerState* m);
bool decode_msg_player_state(PacketReader* r, MsgPlayerState* m);
bool encode_msg_player_left(PacketWriter* w, const MsgPlayerLeft* m);
bool decode_msg_player_left(PacketReader* r, MsgPlayerLeft* m);
bool encode_msg_interest_set(PacketWriter* w, const MsgInterestSet* m, const CellId* cells);
bool decode_msg_interest_set(PacketReader* r, MsgInterestSet* m, CellId* cells_out, uint16_t max_cells);
bool encode_msg_bubble_violation(PacketWriter* w, const MsgBubbleViolation* m);
bool decode_msg_bubble_violation(PacketReader* r, MsgBubbleViolation* m);

bool encode_msg_actor_spawn(PacketWriter* w, const MsgActorSpawn* m);
bool decode_msg_actor_spawn(PacketReader* r, MsgActorSpawn* m);
bool encode_msg_actor_state(PacketWriter* w, const MsgActorState* m);
bool decode_msg_actor_state(PacketReader* r, MsgActorState* m);
bool encode_msg_actor_despawn(PacketWriter* w, const MsgActorDespawn* m);
bool decode_msg_actor_despawn(PacketReader* r, MsgActorDespawn* m);

bool encode_msg_fire_intent(PacketWriter* w, const MsgFireIntent* m);
bool decode_msg_fire_intent(PacketReader* r, MsgFireIntent* m);
bool encode_msg_melee_intent(PacketWriter* w, const MsgMeleeIntent* m);
bool decode_msg_melee_intent(PacketReader* r, MsgMeleeIntent* m);
bool encode_msg_damage_result(PacketWriter* w, const MsgDamageResult* m);
bool decode_msg_damage_result(PacketReader* r, MsgDamageResult* m);
bool encode_msg_death_event(PacketWriter* w, const MsgDeathEvent* m);
bool decode_msg_death_event(PacketReader* r, MsgDeathEvent* m);

bool encode_msg_build_request(PacketWriter* w, const MsgBuildRequest* m);
bool decode_msg_build_request(PacketReader* r, MsgBuildRequest* m);
bool encode_msg_build_result(PacketWriter* w, const MsgBuildResult* m);
bool decode_msg_build_result(PacketReader* r, MsgBuildResult* m);
bool encode_msg_object_spawn(PacketWriter* w, const MsgObjectSpawn* m);
bool decode_msg_object_spawn(PacketReader* r, MsgObjectSpawn* m);
bool encode_msg_object_move_request(PacketWriter* w, const MsgObjectMoveRequest* m);
bool decode_msg_object_move_request(PacketReader* r, MsgObjectMoveRequest* m);
bool encode_msg_object_scrap_request(PacketWriter* w, const MsgObjectScrapRequest* m);
bool decode_msg_object_scrap_request(PacketReader* r, MsgObjectScrapRequest* m);
bool encode_msg_object_update(PacketWriter* w, const MsgObjectUpdate* m);
bool decode_msg_object_update(PacketReader* r, MsgObjectUpdate* m);
bool encode_msg_object_despawn(PacketWriter* w, const MsgObjectDespawn* m);
bool decode_msg_object_despawn(PacketReader* r, MsgObjectDespawn* m);

bool encode_msg_dialogue_open(PacketWriter* w, const MsgDialogueOpen* m);
bool decode_msg_dialogue_open(PacketReader* r, MsgDialogueOpen* m);
bool encode_msg_dialogue_line(PacketWriter* w, const MsgDialogueLine* m, const char* text);
bool decode_msg_dialogue_line(PacketReader* r, MsgDialogueLine* m, char* text_out, size_t text_cap);
bool encode_msg_dialogue_choices(PacketWriter* w, const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, const char* const* texts);
bool decode_msg_dialogue_choices(PacketReader* r, MsgDialogueChoicesHeader* h, MsgDialogueChoiceRecord* choices_out, char texts_out[][256], uint16_t max_choices);
bool encode_msg_dialogue_select(PacketWriter* w, const MsgDialogueSelect* m);
bool decode_msg_dialogue_select(PacketReader* r, MsgDialogueSelect* m);
bool encode_msg_dialogue_close(PacketWriter* w, const MsgDialogueClose* m);
bool decode_msg_dialogue_close(PacketReader* r, MsgDialogueClose* m);
bool encode_msg_quest_update(PacketWriter* w, const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, const char* const* texts);
bool decode_msg_quest_update(PacketReader* r, MsgQuestUpdateHeader* h, MsgQuestObjectiveRecord* objs_out, char texts_out[][256], uint8_t max_objectives);

