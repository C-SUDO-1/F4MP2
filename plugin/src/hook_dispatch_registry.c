#include "hook_dispatch_registry.h"
#include "fo4_bridge_ingress.h"
#include <string.h>
#include <stdio.h>

static HookDispatchStats g_stats;

void hdr_init(void) {
    memset(&g_stats, 0, sizeof(g_stats));
}

bool hdr_dispatch_player_event(const PlayerHostEvent* ev) {
    MsgPlayerState msg;
    if (!phe_translate_player_state(ev, &msg) || !fbi_ingest_remote_player_state(&msg)) {
        g_stats.failures++;
        return false;
    }
    g_stats.player_events++;
    return true;
}

bool hdr_dispatch_actor_event(const ActorHostEvent* ev) {
    MsgActorSpawn spawn; MsgActorState state; MsgActorDespawn despawn; MsgDamageResult dmg; MsgDeathEvent death; bool ok = false;
    if (!ev) { g_stats.failures++; return false; }
    switch (ev->kind) {
        case AHE_SPAWN: ok = ahe_translate_spawn(ev, &spawn) && fbi_ingest_actor_spawn(&spawn); break;
        case AHE_UPDATE: ok = ahe_translate_state(ev, &state) && fbi_ingest_actor_state(&state); break;
        case AHE_DESPAWN: ok = ahe_translate_despawn(ev, &despawn) && fbi_ingest_actor_despawn(&despawn); break;
        case AHE_DAMAGE: ok = ahe_translate_damage(ev, &dmg) && fbi_ingest_damage_result(&dmg); break;
        case AHE_DEATH: ok = ahe_translate_death(ev, &death) && fbi_ingest_death_event(&death); break;
        default: ok = false; break;
    }
    if (!ok) { g_stats.failures++; return false; }
    g_stats.actor_events++; return true;
}

bool hdr_dispatch_workshop_event(const WorkshopHostEvent* ev) {
    MsgObjectSpawn spawn; MsgObjectUpdate update; MsgObjectDespawn despawn; bool ok = false;
    if (!ev) { g_stats.failures++; return false; }
    switch (ev->kind) {
        case WHE_PLACE: ok = whe_translate_spawn(ev, &spawn) && fbi_ingest_object_spawn(&spawn); break;
        case WHE_MOVE: ok = whe_translate_update(ev, &update) && fbi_ingest_object_update(&update); break;
        case WHE_SCRAP: ok = whe_translate_despawn(ev, &despawn) && fbi_ingest_object_despawn(&despawn); break;
        default: ok = false; break;
    }
    if (!ok) { g_stats.failures++; return false; }
    g_stats.workshop_events++; return true;
}

bool hdr_dispatch_dialogue_event(const DialogueHostLineEvent* ev) {
    MsgDialogueOpen open; MsgDialogueLine line; bool ok = false; const char* text = 0;
    if (!ev) { g_stats.failures++; return false; }
    if (ev->text && ev->text[0] != '\0') {
        ok = dqet_translate_line(ev, &line, &text) && fbi_ingest_dialogue_line(&line, text);
    } else {
        ok = dqet_translate_open(ev, &open) && fbi_ingest_dialogue_open(&open);
    }
    if (!ok) { g_stats.failures++; return false; }
    g_stats.dialogue_events++; return true;
}

bool hdr_dispatch_quest_event(const QuestHostObjectiveEvent* ev) {
    MsgQuestUpdateHeader h; MsgQuestObjectiveRecord objs[16]; const char* texts_in[16]; char texts[16][256]; unsigned i; bool ok;
    if (!ev) { g_stats.failures++; return false; }
    memset(&h, 0, sizeof(h));
    memset(objs, 0, sizeof(objs));
    memset(texts, 0, sizeof(texts));
    for (i = 0; i < 16; ++i) {
        texts_in[i] = NULL;
    }
    ok = dqet_translate_quest(ev, &h, objs, texts_in, 16);
    if (ok) {
        for (i = 0; i < h.objective_count && i < 16; ++i) {
            if (texts_in[i]) {
                snprintf(texts[i], sizeof(texts[i]), "%s", texts_in[i]);
            }
        }
        ok = fbi_ingest_quest_update(&h, objs, texts);
    }
    if (!ok) { g_stats.failures++; return false; }
    g_stats.quest_events++; return true;
}

HookDispatchStats hdr_get_stats(void) { return g_stats; }
