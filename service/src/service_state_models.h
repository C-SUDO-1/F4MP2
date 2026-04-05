#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "protocol_types.h"
#include "messages_player.h"
#include "messages_actor.h"
#include "messages_workshop.h"
#include "messages_combat.h"
#include "messages_ui.h"
#include "session_rules.h"
#include "player_state_relay_policy.h"
#include "world_trace_provider.h"

#define F4MP_MAX_PLAYERS_PER_SESSION   8
#define F4MP_MAX_ACTORS_PER_SESSION    4096
#define F4MP_MAX_OBJECTS_PER_SESSION   8192
#define F4MP_MAX_SETTLEMENTS           64
#define F4MP_MAX_PROFILE_ITEMS         16
#define F4MP_MAX_PROFILE_PERKS         16
#define F4MP_MAX_DIALOGUE_CHOICES      12
#define F4MP_MAX_QUEST_OBJECTIVES      16

typedef struct PlayerProfileRef {
    char player_guid[64];
    char display_name[64];
} PlayerProfileRef;

typedef struct PlayerProfileItemSnapshot {
    uint32_t form_id;
    uint32_t count;
    uint8_t  equipped;
} PlayerProfileItemSnapshot;

typedef struct PlayerProfilePerkSnapshot {
    uint32_t perk_form_id;
    uint8_t  rank;
} PlayerProfilePerkSnapshot;

typedef struct PlayerProfileSnapshot {
    bool     loaded;
    uint32_t level;
    uint32_t xp;
    uint32_t equipped_form_id;
    uint32_t caps;
    uint8_t  str_stat;
    uint8_t  per_stat;
    uint8_t  end_stat;
    uint8_t  cha_stat;
    uint8_t  int_stat;
    uint8_t  agi_stat;
    uint8_t  luk_stat;
    uint16_t item_count;
    PlayerProfileItemSnapshot items[F4MP_MAX_PROFILE_ITEMS];
    uint16_t perk_count;
    PlayerProfilePerkSnapshot perks[F4MP_MAX_PROFILE_PERKS];
    uint8_t  sex;
    uint32_t body_preset;
} PlayerProfileSnapshot;

typedef struct PlayerRuntimeState {
    bool     connected;
    PlayerId player_id;
    uint32_t connection_id;
    MsgPlayerState replicated_state;
    Vec3f    last_authoritative_position;
    Rot3f    last_authoritative_rotation;
    uint32_t last_input_tick;
    uint32_t last_recv_sequence;
    uint32_t last_sent_sequence;
    uint32_t last_heard_tick;
    PlayerStateRelayStats relay_stats;
    bool     bubble_violation_active;
    uint8_t  bubble_violation_mode;
    PlayerProfileRef      profile_ref;
    PlayerProfileSnapshot profile_snapshot;
} PlayerRuntimeState;

typedef struct ActorRuntimeState {
    bool         in_use;
    ActorNetId   actor_net_id;
    uint32_t     base_form_id;
    MsgActorState replicated_state;
    uint8_t      actor_kind;
    uint16_t     spawn_flags;
    bool         authority_owned_by_host;
    bool         pending_despawn;
} ActorRuntimeState;

typedef struct ObjectRuntimeState {
    bool         in_use;
    ObjectNetId  object_net_id;
    uint32_t     form_id;
    uint32_t     settlement_id;
    Vec3f        position;
    Rot3f        rotation;
    uint16_t     state_flags;
    PlayerId     owner_player_id;
    bool         pending_despawn;
} ObjectRuntimeState;

typedef struct SettlementState {
    bool      in_use;
    uint32_t  settlement_id;
    Vec3f     center;
    float     allowed_radius;
    uint32_t  object_count;
    uint32_t  object_budget;
} SettlementState;

typedef struct BubbleState {
    Vec3f     center;
    float     radius;
    uint16_t  cell_count;
    CellId    cells[128];
} BubbleState;

typedef struct SessionDialogueChoiceState {
    uint32_t choice_id;
    uint8_t enabled;
    char text[256];
} SessionDialogueChoiceState;

typedef struct SessionDialogueState {
    bool active;
    DialogueId dialogue_id;
    ActorNetId npc_actor_net_id;
    ActorNetId speaker_actor_net_id;
    char current_line[256];
    uint16_t choice_count;
    SessionDialogueChoiceState choices[F4MP_MAX_DIALOGUE_CHOICES];
    uint32_t last_selected_choice_id;
    PlayerId last_selected_by_player;
    uint8_t close_reason;
} SessionDialogueState;

typedef struct SessionQuestObjectiveState {
    ObjectiveId objective_id;
    uint8_t state;
    char text[256];
} SessionQuestObjectiveState;

typedef struct SessionQuestState {
    bool loaded;
    QuestId quest_id;
    uint16_t stage;
    uint8_t tracked;
    uint8_t objective_count;
    SessionQuestObjectiveState objectives[F4MP_MAX_QUEST_OBJECTIVES];
} SessionQuestState;

typedef struct SessionState {
    bool         in_use;
    SessionId    session_id;
    uint32_t     rules;
    uint16_t     tick_rate;
    uint32_t     session_seed;
    PlayerId     host_player_id;
    uint16_t     connected_count;
    BubbleState  bubble;
    WorldTraceProvider trace_provider;
    SessionDialogueState dialogue;
    SessionQuestState quest;
    PlayerRuntimeState players[F4MP_MAX_PLAYERS_PER_SESSION];
    ActorRuntimeState  actors[F4MP_MAX_ACTORS_PER_SESSION];
    ObjectRuntimeState objects[F4MP_MAX_OBJECTS_PER_SESSION];
    SettlementState    settlements[F4MP_MAX_SETTLEMENTS];
} SessionState;
