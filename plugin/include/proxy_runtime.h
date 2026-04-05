#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "plugin_proxy_actor_contract.h"
#include "messages_workshop.h"
#include "messages_control.h"
#include "messages_combat.h"
#include "messages_ui.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PLUGIN_MAX_PROXY_PLAYERS 8
#define PLUGIN_MAX_PROXY_ACTORS 128
#define PLUGIN_MAX_PROXY_OBJECTS 256

typedef struct ProxyPlayerRecord {
    bool in_use;
    PlayerId player_id;
    NetId proxy_net_id;
    uint32_t proxy_base_form_id;
    Vec3f position;
    Rot3f rotation;
    Vec3f velocity;
    uint8_t stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
    uint32_t last_state_sequence;
    uint32_t last_observed_tick;
} ProxyPlayerRecord;

typedef struct ProxyObjectRecord {
    bool in_use;
    ObjectNetId object_net_id;
    uint32_t form_id;
    Vec3f position;
    Rot3f rotation;
    uint16_t state_flags;
} ProxyObjectRecord;

typedef struct ProxyProfileItemState {
    uint32_t form_id;
    uint32_t count;
    uint8_t  equipped;
} ProxyProfileItemState;

typedef struct ProxyProfilePerkState {
    uint32_t perk_form_id;
    uint8_t  rank;
} ProxyProfilePerkState;

typedef struct ProxyProfileState {
    bool loaded;
    MsgProfileSnapshot snapshot;
    uint16_t item_count;
    ProxyProfileItemState items[16];
    uint16_t perk_count;
    ProxyProfilePerkState perks[16];
    MsgProfileAppearance appearance;
} ProxyProfileState;

typedef struct ProxyDialogueChoiceState {
    uint32_t choice_id;
    uint8_t enabled;
    char text[256];
} ProxyDialogueChoiceState;

typedef struct ProxyDialogueState {
    bool active;
    DialogueId dialogue_id;
    ActorNetId npc_actor_net_id;
    ActorNetId speaker_actor_net_id;
    char current_line[256];
    uint16_t choice_count;
    ProxyDialogueChoiceState choices[12];
    uint32_t last_selected_choice_id;
    uint8_t close_reason;
} ProxyDialogueState;

typedef struct ProxyQuestObjectiveState {
    ObjectiveId objective_id;
    uint8_t state;
    char text[256];
} ProxyQuestObjectiveState;

typedef struct ProxyQuestState {
    bool loaded;
    QuestId quest_id;
    uint16_t stage;
    uint8_t tracked;
    uint8_t objective_count;
    ProxyQuestObjectiveState objectives[16];
} ProxyQuestState;

typedef struct ProxyCombatState {
    bool loaded;
    MsgDamageResult last_damage;
    bool has_last_death;
    MsgDeathEvent last_death;
} ProxyCombatState;

typedef struct ProxyActorRecord {
    bool in_use;
    ActorNetId actor_net_id;
    uint32_t base_form_id;
    uint8_t actor_kind;
    uint16_t flags;
    Vec3f position;
    Rot3f rotation;
    float health_norm;
    uint16_t anim_state;
    uint16_t actor_flags;
    NetId target_net_id;
    bool is_dead;
    uint32_t last_damage_from;
    float last_damage_amount;
} ProxyActorRecord;

void proxy_runtime_init(void);
size_t proxy_runtime_player_count(void);
size_t proxy_runtime_actor_count(void);
size_t proxy_runtime_object_count(void);
const ProxyPlayerRecord* proxy_runtime_get_player(PlayerId player_id);
const ProxyActorRecord* proxy_runtime_get_actor(ActorNetId actor_net_id);
const ProxyObjectRecord* proxy_runtime_get_object(ObjectNetId object_net_id);
const ProxyProfileState* proxy_runtime_get_profile(void);
const ProxyCombatState* proxy_runtime_get_combat(void);
const ProxyDialogueState* proxy_runtime_get_dialogue(void);
const ProxyQuestState* proxy_runtime_get_quest(void);

bool proxy_spawn_object(const MsgObjectSpawn* msg);
bool proxy_update_object(const MsgObjectUpdate* msg);
bool proxy_despawn_object(ObjectNetId object_net_id);
void proxy_set_profile_snapshot(const MsgProfileSnapshot* msg);
void proxy_set_profile_items(const MsgProfileItemsHeader* h, const MsgProfileItemRecord* items);
void proxy_set_profile_perks(const MsgProfilePerksHeader* h, const MsgProfilePerkRecord* perks);
void proxy_set_profile_appearance(const MsgProfileAppearance* msg);
void proxy_set_damage_result(const MsgDamageResult* msg);
void proxy_set_death_event(const MsgDeathEvent* msg);

void proxy_set_dialogue_open(const MsgDialogueOpen* msg);
void proxy_set_dialogue_line(const MsgDialogueLine* msg, const char* text);
void proxy_set_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]);
void proxy_note_dialogue_select(const MsgDialogueSelect* msg);
void proxy_set_dialogue_close(const MsgDialogueClose* msg);
void proxy_set_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]);

bool proxy_spawn_player(const ProxyPlayerSpawnSpec* spec);
bool proxy_update_player(const ProxyPlayerUpdateSpec* spec);
bool proxy_despawn_player(PlayerId player_id);

bool proxy_spawn_actor(const ProxyActorSpawnSpec* spec);
bool proxy_update_actor(const ProxyActorUpdateSpec* spec);
bool proxy_despawn_actor(ActorNetId actor_net_id);

#ifdef __cplusplus
}
#endif
