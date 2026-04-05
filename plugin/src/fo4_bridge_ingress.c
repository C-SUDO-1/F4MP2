#include "fo4_bridge_ingress.h"
#include "protocol_apply.h"
#include <string.h>

static MsgPlayerState g_last_player_state;
static bool g_has_last_player_state = false;

void fbi_init(void) {
    memset(&g_last_player_state, 0, sizeof(g_last_player_state));
    g_has_last_player_state = false;
}

bool fbi_ingest_remote_player_state(const MsgPlayerState* msg) {
    if (!msg) return false;
    g_last_player_state = *msg;
    g_has_last_player_state = true;
    return pa_apply_player_state(msg);
}

bool fbi_ingest_actor_spawn(const MsgActorSpawn* msg) { return pa_apply_actor_spawn(msg); }
bool fbi_ingest_actor_state(const MsgActorState* msg) { return pa_apply_actor_state(msg); }
bool fbi_ingest_actor_despawn(const MsgActorDespawn* msg) { return pa_apply_actor_despawn(msg); }

bool fbi_ingest_object_spawn(const MsgObjectSpawn* msg) { return pa_apply_object_spawn(msg); }
bool fbi_ingest_object_update(const MsgObjectUpdate* msg) { return pa_apply_object_update(msg); }
bool fbi_ingest_object_despawn(const MsgObjectDespawn* msg) { return pa_apply_object_despawn(msg); }

bool fbi_ingest_damage_result(const MsgDamageResult* msg) { return pa_apply_damage_result(msg); }
bool fbi_ingest_death_event(const MsgDeathEvent* msg) { return pa_apply_death_event(msg); }

bool fbi_ingest_dialogue_open(const MsgDialogueOpen* msg) { return pa_apply_dialogue_open(msg); }
bool fbi_ingest_dialogue_line(const MsgDialogueLine* msg, const char* text) { return pa_apply_dialogue_line(msg, text); }
bool fbi_ingest_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]) { return pa_apply_dialogue_choices(h, choices, texts); }
bool fbi_ingest_dialogue_close(const MsgDialogueClose* msg) { return pa_apply_dialogue_close(msg); }
bool fbi_ingest_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]) { return pa_apply_quest_update(h, objs, texts); }

bool fbi_get_last_player_state(MsgPlayerState* out) {
    if (!out || !g_has_last_player_state) return false;
    *out = g_last_player_state;
    return true;
}
