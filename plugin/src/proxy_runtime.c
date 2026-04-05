#include "proxy_runtime.h"
#include <string.h>

static ProxyPlayerRecord g_players[PLUGIN_MAX_PROXY_PLAYERS];
static ProxyActorRecord g_actors[PLUGIN_MAX_PROXY_ACTORS];
static ProxyObjectRecord g_objects[PLUGIN_MAX_PROXY_OBJECTS];
static ProxyProfileState g_profile;
static ProxyCombatState g_combat;
static ProxyDialogueState g_dialogue;
static ProxyQuestState g_quest;

void proxy_runtime_init(void) {
    memset(g_players, 0, sizeof(g_players));
    memset(g_actors, 0, sizeof(g_actors));
    memset(g_objects, 0, sizeof(g_objects));
    memset(&g_profile, 0, sizeof(g_profile));
    memset(&g_combat, 0, sizeof(g_combat));
    memset(&g_dialogue, 0, sizeof(g_dialogue));
    memset(&g_quest, 0, sizeof(g_quest));
}

static ProxyPlayerRecord* find_player_slot(PlayerId player_id) {
    size_t i;
    for (i = 0; i < PLUGIN_MAX_PROXY_PLAYERS; ++i) if (g_players[i].in_use && g_players[i].player_id == player_id) return &g_players[i];
    for (i = 0; i < PLUGIN_MAX_PROXY_PLAYERS; ++i) if (!g_players[i].in_use) return &g_players[i];
    return NULL;
}

static ProxyObjectRecord* find_object_slot(ObjectNetId object_net_id) {
    size_t i;
    for (i = 0; i < PLUGIN_MAX_PROXY_OBJECTS; ++i) if (g_objects[i].in_use && g_objects[i].object_net_id == object_net_id) return &g_objects[i];
    for (i = 0; i < PLUGIN_MAX_PROXY_OBJECTS; ++i) if (!g_objects[i].in_use) return &g_objects[i];
    return NULL;
}

static ProxyActorRecord* find_actor_slot(ActorNetId actor_net_id) {
    size_t i;
    for (i = 0; i < PLUGIN_MAX_PROXY_ACTORS; ++i) if (g_actors[i].in_use && g_actors[i].actor_net_id == actor_net_id) return &g_actors[i];
    for (i = 0; i < PLUGIN_MAX_PROXY_ACTORS; ++i) if (!g_actors[i].in_use) return &g_actors[i];
    return NULL;
}

bool proxy_spawn_player(const ProxyPlayerSpawnSpec* spec) {
    ProxyPlayerRecord* s;
    if (!spec) return false;
    s = find_player_slot(spec->player_id);
    if (!s) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->player_id = spec->player_id;
    s->proxy_net_id = spec->proxy_net_id;
    s->proxy_base_form_id = spec->proxy_base_form_id;
    s->position = spec->position;
    s->rotation = spec->rotation;
    s->equipped_form_id = spec->equipped_form_id;
    return true;
}

bool proxy_update_player(const ProxyPlayerUpdateSpec* spec) {
    ProxyPlayerRecord* s;
    if (!spec) return false;
    s = find_player_slot(spec->player_id);
    if (!s || !s->in_use) return false;
    s->position = spec->position;
    s->rotation = spec->rotation;
    s->velocity = spec->velocity;
    s->stance = spec->stance;
    s->anim_state = spec->anim_state;
    s->equipped_form_id = spec->equipped_form_id;
    s->last_state_sequence = 0;
    s->last_observed_tick = 0;
    return true;
}

bool proxy_despawn_player(PlayerId player_id) {
    ProxyPlayerRecord* s = find_player_slot(player_id);
    if (!s || !s->in_use || s->player_id != player_id) return false;
    memset(s, 0, sizeof(*s));
    return true;
}

bool proxy_spawn_actor(const ProxyActorSpawnSpec* spec) {
    ProxyActorRecord* s;
    if (!spec) return false;
    s = find_actor_slot(spec->actor_net_id);
    if (!s) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->actor_net_id = spec->actor_net_id;
    s->base_form_id = spec->base_form_id;
    s->position = spec->position;
    s->rotation = spec->rotation;
    s->actor_kind = spec->actor_kind;
    s->flags = spec->flags;
    s->health_norm = 1.0f;
    return true;
}

bool proxy_update_actor(const ProxyActorUpdateSpec* spec) {
    ProxyActorRecord* s;
    if (!spec) return false;
    s = find_actor_slot(spec->actor_net_id);
    if (!s || !s->in_use) return false;
    s->position = spec->position;
    s->rotation = spec->rotation;
    s->health_norm = spec->health_norm;
    s->anim_state = spec->anim_state;
    s->actor_flags = spec->actor_flags;
    s->target_net_id = spec->target_net_id;
    if (spec->health_norm > 0.0f) s->is_dead = false;
    return true;
}

bool proxy_despawn_actor(ActorNetId actor_net_id) {
    ProxyActorRecord* s = find_actor_slot(actor_net_id);
    if (!s || !s->in_use || s->actor_net_id != actor_net_id) return false;
    if (g_dialogue.active && (g_dialogue.npc_actor_net_id == actor_net_id || g_dialogue.speaker_actor_net_id == actor_net_id)) {
        g_dialogue.active = false;
        g_dialogue.close_reason = DLG_CLOSE_INTERRUPTED;
        g_dialogue.choice_count = 0;
        g_dialogue.current_line[0] = '\0';
    }
    memset(s, 0, sizeof(*s));
    return true;
}

size_t proxy_runtime_player_count(void) { size_t i, n = 0; for (i = 0; i < PLUGIN_MAX_PROXY_PLAYERS; ++i) if (g_players[i].in_use) ++n; return n; }
size_t proxy_runtime_actor_count(void) { size_t i, n = 0; for (i = 0; i < PLUGIN_MAX_PROXY_ACTORS; ++i) if (g_actors[i].in_use) ++n; return n; }
size_t proxy_runtime_object_count(void) { size_t i, n = 0; for (i = 0; i < PLUGIN_MAX_PROXY_OBJECTS; ++i) if (g_objects[i].in_use) ++n; return n; }

const ProxyPlayerRecord* proxy_runtime_get_player(PlayerId player_id) { size_t i; for (i = 0; i < PLUGIN_MAX_PROXY_PLAYERS; ++i) if (g_players[i].in_use && g_players[i].player_id == player_id) return &g_players[i]; return NULL; }
const ProxyActorRecord* proxy_runtime_get_actor(ActorNetId actor_net_id) { size_t i; for (i = 0; i < PLUGIN_MAX_PROXY_ACTORS; ++i) if (g_actors[i].in_use && g_actors[i].actor_net_id == actor_net_id) return &g_actors[i]; return NULL; }
const ProxyObjectRecord* proxy_runtime_get_object(ObjectNetId object_net_id) { size_t i; for (i = 0; i < PLUGIN_MAX_PROXY_OBJECTS; ++i) if (g_objects[i].in_use && g_objects[i].object_net_id == object_net_id) return &g_objects[i]; return NULL; }
const ProxyProfileState* proxy_runtime_get_profile(void) { return &g_profile; }
const ProxyCombatState* proxy_runtime_get_combat(void) { return &g_combat; }
const ProxyDialogueState* proxy_runtime_get_dialogue(void) { return &g_dialogue; }
const ProxyQuestState* proxy_runtime_get_quest(void) { return &g_quest; }

bool proxy_spawn_object(const MsgObjectSpawn* msg) {
    ProxyObjectRecord* s;
    if (!msg) return false;
    s = find_object_slot(msg->object_net_id);
    if (!s) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->object_net_id = msg->object_net_id;
    s->form_id = msg->form_id;
    s->position = msg->position;
    s->rotation = msg->rotation;
    s->state_flags = msg->state_flags;
    return true;
}

bool proxy_update_object(const MsgObjectUpdate* msg) {
    ProxyObjectRecord* s;
    if (!msg) return false;
    s = find_object_slot(msg->object_net_id);
    if (!s || !s->in_use) return false;
    if (msg->fields_mask & OUF_POSITION) s->position = msg->position;
    if (msg->fields_mask & OUF_ROTATION) s->rotation = msg->rotation;
    if (msg->fields_mask & OUF_STATE_FLAGS) s->state_flags = msg->state_flags;
    return true;
}

bool proxy_despawn_object(ObjectNetId object_net_id) {
    ProxyObjectRecord* s = find_object_slot(object_net_id);
    if (!s || !s->in_use || s->object_net_id != object_net_id) return false;
    memset(s, 0, sizeof(*s));
    return true;
}

void proxy_set_profile_snapshot(const MsgProfileSnapshot* msg) { if (msg) { g_profile.loaded = true; g_profile.snapshot = *msg; } }
void proxy_set_profile_items(const MsgProfileItemsHeader* h, const MsgProfileItemRecord* items) {
    uint16_t i, count = (h && h->item_count < 16) ? h->item_count : 16;
    g_profile.item_count = count;
    for (i = 0; i < count; ++i) { g_profile.items[i].form_id = items[i].form_id; g_profile.items[i].count = items[i].count; g_profile.items[i].equipped = items[i].equipped; }
}
void proxy_set_profile_perks(const MsgProfilePerksHeader* h, const MsgProfilePerkRecord* perks) {
    uint16_t i, count = (h && h->perk_count < 16) ? h->perk_count : 16;
    g_profile.perk_count = count;
    for (i = 0; i < count; ++i) { g_profile.perks[i].perk_form_id = perks[i].perk_form_id; g_profile.perks[i].rank = perks[i].rank; }
}
void proxy_set_profile_appearance(const MsgProfileAppearance* msg) { if (msg) g_profile.appearance = *msg; }

void proxy_set_damage_result(const MsgDamageResult* msg) {
    ProxyActorRecord* actor;
    if (!msg) return;
    g_combat.loaded = true;
    g_combat.last_damage = *msg;
    actor = find_actor_slot(msg->victim_actor_net_id);
    if (actor && actor->in_use) {
        actor->health_norm = msg->health_after;
        actor->last_damage_from = msg->attacker_id;
        actor->last_damage_amount = msg->damage_amount;
        if ((msg->flags & DMGF_KILLED) || msg->health_after <= 0.0f) actor->is_dead = true;
    }
}

void proxy_set_death_event(const MsgDeathEvent* msg) {
    ProxyActorRecord* actor;
    if (!msg) return;
    g_combat.has_last_death = true;
    g_combat.last_death = *msg;
    actor = find_actor_slot(msg->victim_actor_net_id);
    if (actor && actor->in_use) {
        actor->is_dead = true;
        actor->health_norm = 0.0f;
    }
    if (g_dialogue.active && (g_dialogue.npc_actor_net_id == msg->victim_actor_net_id || g_dialogue.speaker_actor_net_id == msg->victim_actor_net_id)) {
        g_dialogue.active = false;
        g_dialogue.close_reason = DLG_CLOSE_INTERRUPTED;
        g_dialogue.choice_count = 0;
        g_dialogue.current_line[0] = '\0';
    }
}

void proxy_set_dialogue_open(const MsgDialogueOpen* msg) {
    if (!msg) return;
    memset(&g_dialogue, 0, sizeof(g_dialogue));
    g_dialogue.active = true;
    g_dialogue.dialogue_id = msg->dialogue_id;
    g_dialogue.npc_actor_net_id = msg->npc_actor_net_id;
    g_dialogue.speaker_actor_net_id = msg->speaker_actor_net_id;
}
void proxy_set_dialogue_line(const MsgDialogueLine* msg, const char* text) {
    if (!msg) return;
    g_dialogue.active = true;
    g_dialogue.dialogue_id = msg->dialogue_id;
    g_dialogue.speaker_actor_net_id = msg->speaker_actor_net_id;
    strncpy(g_dialogue.current_line, text ? text : "", sizeof(g_dialogue.current_line) - 1);
}
void proxy_set_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]) {
    uint16_t i, count;
    if (!h) return;
    g_dialogue.active = true;
    g_dialogue.dialogue_id = h->dialogue_id;
    count = (h->choice_count < 12) ? h->choice_count : 12;
    g_dialogue.choice_count = count;
    for (i = 0; i < count; ++i) {
        g_dialogue.choices[i].choice_id = choices[i].choice_id;
        g_dialogue.choices[i].enabled = choices[i].enabled;
        strncpy(g_dialogue.choices[i].text, texts[i], sizeof(g_dialogue.choices[i].text) - 1);
    }
}
void proxy_note_dialogue_select(const MsgDialogueSelect* msg) {
    if (!msg) return;
    if (g_dialogue.active && g_dialogue.dialogue_id == msg->dialogue_id) g_dialogue.last_selected_choice_id = msg->choice_id;
}
void proxy_set_dialogue_close(const MsgDialogueClose* msg) {
    if (!msg) return;
    if (g_dialogue.dialogue_id == msg->dialogue_id) {
        g_dialogue.active = false;
        g_dialogue.close_reason = msg->reason;
        g_dialogue.choice_count = 0;
        g_dialogue.current_line[0] = '\0';
    }
}
void proxy_set_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]) {
    uint8_t i, count;
    if (!h) return;
    memset(&g_quest, 0, sizeof(g_quest));
    g_quest.loaded = true;
    g_quest.quest_id = h->quest_id;
    g_quest.stage = h->stage;
    g_quest.tracked = h->tracked;
    count = (h->objective_count < 16) ? h->objective_count : 16;
    g_quest.objective_count = count;
    for (i = 0; i < count; ++i) {
        g_quest.objectives[i].objective_id = objs[i].objective_id;
        g_quest.objectives[i].state = objs[i].state;
        strncpy(g_quest.objectives[i].text, texts[i], sizeof(g_quest.objectives[i].text) - 1);
    }
}
