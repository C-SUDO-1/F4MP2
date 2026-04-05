#include "transport_loop.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "authority_core.h"
#include "message_ids.h"
#include "messages_control.h"
#include "messages_player.h"
#include "messages_combat.h"
#include "messages_workshop.h"
#include "packet_codec.h"
#include "message_validation.h"
#include "protocol_constants.h"
#include "replay_log.h"
#include "service_indices.h"
#include "profile_store.h"
#include "session_manager.h"
#include "session_registry.h"
#include "world_state_store.h"
#include "messages_ui.h"
#include "dialogue_selection_policy.h"
#include "player_state_relay_policy.h"

typedef bool (*EncodeFn)(PacketWriter* w, const void* msg);

static bool tl_push_output(TransportLoopContext* ctx, uint32_t connection_id, const uint8_t* bytes, size_t length) {
    if (!ctx || !bytes || length == 0 || length > sizeof(ctx->outputs[0].bytes)) return false;
    if (ctx->output_count >= (sizeof(ctx->outputs) / sizeof(ctx->outputs[0]))) return false;
    ctx->outputs[ctx->output_count].connection_id = connection_id;
    ctx->outputs[ctx->output_count].length = length;
    memcpy(ctx->outputs[ctx->output_count].bytes, bytes, length);
    ctx->output_count++;
    return true;
}

static bool encode_build_result_adapter(PacketWriter* w, const void* msg) { return encode_msg_build_result(w, (const MsgBuildResult*)msg); }
static bool encode_object_spawn_adapter(PacketWriter* w, const void* msg) { return encode_msg_object_spawn(w, (const MsgObjectSpawn*)msg); }
static bool encode_object_update_adapter(PacketWriter* w, const void* msg) { return encode_msg_object_update(w, (const MsgObjectUpdate*)msg); }
static bool encode_object_despawn_adapter(PacketWriter* w, const void* msg) { return encode_msg_object_despawn(w, (const MsgObjectDespawn*)msg); }
static bool encode_player_state_adapter(PacketWriter* w, const void* msg) { return encode_msg_player_state(w, (const MsgPlayerState*)msg); }
static bool encode_player_left_adapter(PacketWriter* w, const void* msg) { return encode_msg_player_left(w, (const MsgPlayerLeft*)msg); }
static bool encode_ping_adapter(PacketWriter* w, const void* msg) { return encode_msg_ping(w, (const MsgPing*)msg); }
static bool encode_pong_adapter(PacketWriter* w, const void* msg) { return encode_msg_pong(w, (const MsgPong*)msg); }
static bool encode_welcome_adapter(PacketWriter* w, const void* msg) { return encode_msg_welcome(w, (const MsgWelcome*)msg); }
static bool encode_profile_snapshot_adapter(PacketWriter* w, const void* msg) { return encode_msg_profile_snapshot(w, (const MsgProfileSnapshot*)msg); }
static bool encode_profile_items_adapter(PacketWriter* w, const void* msg) { const MsgProfileItemsHeader* h = (const MsgProfileItemsHeader*)msg; const MsgProfileItemRecord* items = (const MsgProfileItemRecord*)(h + 1); return encode_msg_profile_items(w, h, items); }
static bool encode_profile_perks_adapter(PacketWriter* w, const void* msg) { const MsgProfilePerksHeader* h = (const MsgProfilePerksHeader*)msg; const MsgProfilePerkRecord* perks = (const MsgProfilePerkRecord*)(h + 1); return encode_msg_profile_perks(w, h, perks); }
static bool encode_profile_appearance_adapter(PacketWriter* w, const void* msg) { return encode_msg_profile_appearance(w, (const MsgProfileAppearance*)msg); }
static bool encode_actor_spawn_adapter(PacketWriter* w, const void* msg) { return encode_msg_actor_spawn(w, (const MsgActorSpawn*)msg); }
static bool encode_actor_state_adapter(PacketWriter* w, const void* msg) { return encode_msg_actor_state(w, (const MsgActorState*)msg); }
static bool encode_actor_despawn_adapter(PacketWriter* w, const void* msg) { return encode_msg_actor_despawn(w, (const MsgActorDespawn*)msg); }
static bool encode_damage_result_adapter(PacketWriter* w, const void* msg) { return encode_msg_damage_result(w, (const MsgDamageResult*)msg); }
static bool encode_death_event_adapter(PacketWriter* w, const void* msg) { return encode_msg_death_event(w, (const MsgDeathEvent*)msg); }
static bool encode_dialogue_open_adapter(PacketWriter* w, const void* msg) { return encode_msg_dialogue_open(w, (const MsgDialogueOpen*)msg); }
static bool encode_dialogue_line_adapter(PacketWriter* w, const void* msg) { const MsgDialogueLine* h = (const MsgDialogueLine*)msg; const char* text = (const char*)(h + 1); return encode_msg_dialogue_line(w, h, text); }
static bool encode_dialogue_choices_adapter(PacketWriter* w, const void* msg) { const MsgDialogueChoicesHeader* h = (const MsgDialogueChoicesHeader*)msg; const MsgDialogueChoiceRecord* choices = (const MsgDialogueChoiceRecord*)(h + 1); const char* const* texts = (const char* const*)(choices + h->choice_count); return encode_msg_dialogue_choices(w, h, choices, texts); }
static bool encode_dialogue_close_adapter(PacketWriter* w, const void* msg) { return encode_msg_dialogue_close(w, (const MsgDialogueClose*)msg); }
static bool encode_quest_update_adapter(PacketWriter* w, const void* msg) { const MsgQuestUpdateHeader* h = (const MsgQuestUpdateHeader*)msg; const MsgQuestObjectiveRecord* objs = (const MsgQuestObjectiveRecord*)(h + 1); const char* const* texts = (const char* const*)(objs + h->objective_count); return encode_msg_quest_update(w, h, objs, texts); }

static bool tl_send_single(TransportLoopContext* ctx, uint32_t connection_id, uint16_t msg_type, uint16_t msg_flags, const void* msg, EncodeFn enc) {
    uint8_t buffer[F4MP_MAX_PACKET_SIZE];
    PacketWriter w;
    PacketHeader ph;
    MessageHeader mh;
    size_t payload_start;
    uint16_t payload_len;

    memset(&ph, 0, sizeof(ph));
    ph.magic = F4MP_MAGIC;
    ph.version = F4MP_VERSION;
    ph.session_id = ctx->session->session_id;
    ph.sequence = ++ctx->transport.last_tx_sequence;
    ph.message_count = 1;

    memset(&mh, 0, sizeof(mh));
    mh.type = msg_type;
    mh.flags = msg_flags;

    pw_init(&w, buffer, sizeof(buffer));
    if (!encode_packet_header(&w, &ph)) return false;
    if (!encode_message_header(&w, &mh)) return false;
    payload_start = pw_bytes_written(&w);
    if (!enc(&w, msg)) return false;
    payload_len = (uint16_t)(pw_bytes_written(&w) - payload_start);
    memcpy(w.buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return tl_push_output(ctx, connection_id, buffer, pw_bytes_written(&w));
}

static void tl_make_safe_guid(const char* guid, char* out, size_t out_cap) {
    size_t i, j = 0;
    if (!out || out_cap == 0) return;
    for (i = 0; guid && guid[i] != '\0' && j + 1 < out_cap; ++i) {
        char c = guid[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-') {
            out[j++] = c;
        } else {
            out[j++] = '_';
        }
    }
    out[j] = '\0';
}

static void tl_profile_path_for_guid(const char* player_guid, char* out, size_t out_cap) {
    char safe_guid[96];
    tl_make_safe_guid(player_guid, safe_guid, sizeof(safe_guid));
    snprintf(out, out_cap, "profiles/%s.json", safe_guid[0] ? safe_guid : "anonymous");
}

static bool tl_send_profile_snapshot(TransportLoopContext* ctx, uint32_t connection_id, const PlayerRuntimeState* player) {
    MsgProfileSnapshot msg;
    struct { MsgProfileItemsHeader h; MsgProfileItemRecord items[16]; } items_msg;
    struct { MsgProfilePerksHeader h; MsgProfilePerkRecord perks[16]; } perks_msg;
    MsgProfileAppearance appearance;
    uint16_t i;
    memset(&msg, 0, sizeof(msg));
    msg.level = player->profile_snapshot.level;
    msg.xp = player->profile_snapshot.xp;
    msg.equipped_form_id = player->profile_snapshot.equipped_form_id;
    msg.caps = player->profile_snapshot.caps;
    msg.str_stat = player->profile_snapshot.str_stat;
    msg.per_stat = player->profile_snapshot.per_stat;
    msg.end_stat = player->profile_snapshot.end_stat;
    msg.cha_stat = player->profile_snapshot.cha_stat;
    msg.int_stat = player->profile_snapshot.int_stat;
    msg.agi_stat = player->profile_snapshot.agi_stat;
    msg.luk_stat = player->profile_snapshot.luk_stat;
    if (!tl_send_single(ctx, connection_id, MSG_PROFILE_SNAPSHOT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, &msg, encode_profile_snapshot_adapter)) return false;
    memset(&items_msg, 0, sizeof(items_msg));
    items_msg.h.item_count = player->profile_snapshot.item_count;
    for (i = 0; i < player->profile_snapshot.item_count && i < 16; ++i) {
        items_msg.items[i].form_id = player->profile_snapshot.items[i].form_id;
        items_msg.items[i].count = player->profile_snapshot.items[i].count;
        items_msg.items[i].equipped = player->profile_snapshot.items[i].equipped;
    }
    if (!tl_send_single(ctx, connection_id, MSG_PROFILE_ITEMS, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, &items_msg, encode_profile_items_adapter)) return false;
    memset(&perks_msg, 0, sizeof(perks_msg));
    perks_msg.h.perk_count = player->profile_snapshot.perk_count;
    for (i = 0; i < player->profile_snapshot.perk_count && i < 16; ++i) {
        perks_msg.perks[i].perk_form_id = player->profile_snapshot.perks[i].perk_form_id;
        perks_msg.perks[i].rank = player->profile_snapshot.perks[i].rank;
    }
    if (!tl_send_single(ctx, connection_id, MSG_PROFILE_PERKS, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, &perks_msg, encode_profile_perks_adapter)) return false;
    memset(&appearance, 0, sizeof(appearance));
    appearance.sex = player->profile_snapshot.sex;
    appearance.body_preset = player->profile_snapshot.body_preset;
    return tl_send_single(ctx, connection_id, MSG_PROFILE_APPEARANCE, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, &appearance, encode_profile_appearance_adapter);
}

static bool tl_send_welcome(TransportLoopContext* ctx, uint32_t connection_id, PlayerId player_id) {
    MsgWelcome msg;
    msg.session_id = ctx->session->session_id;
    msg.player_id = player_id;
    msg.tick_rate = ctx->session->tick_rate;
    msg.rules = ctx->session->rules;
    return tl_send_single(ctx, connection_id, MSG_WELCOME, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, &msg, encode_welcome_adapter);
}


static void tl_broadcast_player_state(TransportLoopContext* ctx, const PlayerRuntimeState* player) {
    size_t i;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_PLAYER_STATE, MSGF_CH1_WORLD, &player->replicated_state, encode_player_state_adapter);
        }
    }
}

static void tl_broadcast_player_left(TransportLoopContext* ctx, PlayerId player_id) {
    size_t i;
    MsgPlayerLeft msg;
    memset(&msg, 0, sizeof(msg));
    msg.player_id = player_id;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_PLAYER_LEFT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH1_WORLD, &msg, encode_player_left_adapter);
        }
    }
}

static void tl_broadcast_actor_spawn(TransportLoopContext* ctx, const ActorRuntimeState* actor) {
    size_t i;
    MsgActorSpawn msg;
    memset(&msg, 0, sizeof(msg));
    msg.actor_net_id = actor->actor_net_id;
    msg.base_form_id = actor->base_form_id;
    msg.position = actor->replicated_state.position;
    msg.rotation = actor->replicated_state.rotation;
    msg.actor_kind = actor->actor_kind;
    msg.flags = actor->spawn_flags;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_ACTOR_SPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH1_WORLD, &msg, encode_actor_spawn_adapter);
        }
    }
}

static void tl_broadcast_actor_state(TransportLoopContext* ctx, const ActorRuntimeState* actor) {
    size_t i;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_ACTOR_STATE, MSGF_CH1_WORLD, &actor->replicated_state, encode_actor_state_adapter);
        }
    }
}

static void tl_broadcast_actor_despawn(TransportLoopContext* ctx, ActorNetId actor_net_id, uint8_t reason) {
    size_t i;
    MsgActorDespawn msg;
    memset(&msg, 0, sizeof(msg));
    msg.actor_net_id = actor_net_id;
    msg.reason = reason;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_ACTOR_DESPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH1_WORLD, &msg, encode_actor_despawn_adapter);
        }
    }
}

static void tl_broadcast_damage(TransportLoopContext* ctx, const MsgDamageResult* msg) {
    size_t i;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DAMAGE_RESULT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH2_COMBAT, msg, encode_damage_result_adapter);
        }
    }
}

static void tl_broadcast_death(TransportLoopContext* ctx, const MsgDeathEvent* msg) {
    size_t i;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DEATH_EVENT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH2_COMBAT, msg, encode_death_event_adapter);
        }
    }
}

static void tl_broadcast_object_spawn(TransportLoopContext* ctx, const ObjectRuntimeState* obj) {
    size_t i;
    MsgObjectSpawn msg;
    memset(&msg, 0, sizeof(msg));
    msg.object_net_id = obj->object_net_id;
    msg.form_id = obj->form_id;
    msg.position = obj->position;
    msg.rotation = obj->rotation;
    msg.state_flags = obj->state_flags;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_OBJECT_SPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, &msg, encode_object_spawn_adapter);
        }
    }
}

static void tl_broadcast_object_update(TransportLoopContext* ctx, const ObjectRuntimeState* obj, uint32_t fields_mask) {
    size_t i;
    MsgObjectUpdate msg;
    memset(&msg, 0, sizeof(msg));
    msg.object_net_id = obj->object_net_id;
    msg.fields_mask = fields_mask;
    msg.position = obj->position;
    msg.rotation = obj->rotation;
    msg.state_flags = obj->state_flags;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_OBJECT_UPDATE, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, &msg, encode_object_update_adapter);
        }
    }
}

static void tl_broadcast_object_despawn(TransportLoopContext* ctx, ObjectNetId object_net_id, uint8_t reason) {
    size_t i;
    MsgObjectDespawn msg;
    memset(&msg, 0, sizeof(msg));
    msg.object_net_id = object_net_id;
    msg.reason = reason;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (ctx->session->players[i].connected) {
            tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_OBJECT_DESPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, &msg, encode_object_despawn_adapter);
        }
    }
}

static void tl_send_existing_state_to_player(TransportLoopContext* ctx, uint32_t connection_id, PlayerId exclude_player_id) {
    size_t i;
    Vec3f replay_center = ctx->session->bubble.center;
    float replay_radius = ctx->session->bubble.radius;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        const PlayerRuntimeState* p = &ctx->session->players[i];
        if (p->connected && p->player_id != exclude_player_id) {
            tl_send_single(ctx, connection_id, MSG_PLAYER_STATE, MSGF_CH1_WORLD, &p->replicated_state, encode_player_state_adapter);
        }
        if (p->connected && p->player_id == exclude_player_id) {
            replay_center = p->replicated_state.position;
        }
    }
    if (replay_radius <= 0.0f) replay_radius = 512.0f;
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
        const ActorRuntimeState* a = &ctx->session->actors[i];
        MsgActorSpawn spawn;
        float dx,dy,dz;
        if (!a->in_use) continue;
        dx = a->replicated_state.position.x - replay_center.x;
        dy = a->replicated_state.position.y - replay_center.y;
        dz = a->replicated_state.position.z - replay_center.z;
        if ((dx*dx + dy*dy + dz*dz) > replay_radius*replay_radius) continue;
        memset(&spawn, 0, sizeof(spawn));
        spawn.actor_net_id = a->actor_net_id;
        spawn.base_form_id = a->base_form_id;
        spawn.position = a->replicated_state.position;
        spawn.rotation = a->replicated_state.rotation;
        spawn.actor_kind = a->actor_kind;
        spawn.flags = a->spawn_flags;
        tl_send_single(ctx, connection_id, MSG_ACTOR_SPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH1_WORLD, &spawn, encode_actor_spawn_adapter);
        tl_send_single(ctx, connection_id, MSG_ACTOR_STATE, MSGF_CH1_WORLD, &a->replicated_state, encode_actor_state_adapter);
    }
    for (i = 0; i < F4MP_MAX_OBJECTS_PER_SESSION; ++i) {
        const ObjectRuntimeState* o = &ctx->session->objects[i];
        MsgObjectSpawn spawn;
        float dx,dy,dz;
        if (!o->in_use) continue;
        dx = o->position.x - replay_center.x;
        dy = o->position.y - replay_center.y;
        dz = o->position.z - replay_center.z;
        if ((dx*dx + dy*dy + dz*dz) > replay_radius*replay_radius) continue;
        memset(&spawn, 0, sizeof(spawn));
        spawn.object_net_id = o->object_net_id;
        spawn.form_id = o->form_id;
        spawn.position = o->position;
        spawn.rotation = o->rotation;
        spawn.state_flags = o->state_flags;
        tl_send_single(ctx, connection_id, MSG_OBJECT_SPAWN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, &spawn, encode_object_spawn_adapter);
    }
}



bool tl_broadcast_dialogue_open(TransportLoopContext* ctx, const MsgDialogueOpen* msg) {
    size_t i; if (!ctx || !msg) return false;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (ctx->session->players[i].connected) tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DIALOGUE_OPEN, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI, msg, encode_dialogue_open_adapter);
    return true;
}
bool tl_broadcast_dialogue_line(TransportLoopContext* ctx, const MsgDialogueLine* msg, const char* text) {
    size_t i; struct { MsgDialogueLine h; char text[256]; } payload; if (!ctx || !msg) return false; memset(&payload,0,sizeof(payload)); payload.h=*msg; if (text) strncpy(payload.text,text,sizeof(payload.text)-1);
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (ctx->session->players[i].connected) tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DIALOGUE_LINE, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI, &payload, encode_dialogue_line_adapter);
    return true;
}
bool tl_broadcast_dialogue_choices(TransportLoopContext* ctx, const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, const char* const* texts) {
    size_t i; struct { MsgDialogueChoicesHeader h; MsgDialogueChoiceRecord choices[12]; const char* texts[12]; } payload; if (!ctx || !h) return false; memset(&payload,0,sizeof(payload)); payload.h=*h; if (payload.h.choice_count>12) payload.h.choice_count=12; for (i=0;i<payload.h.choice_count;++i){ payload.choices[i]=choices[i]; payload.texts[i]=texts?texts[i]:""; }
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (ctx->session->players[i].connected) tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DIALOGUE_CHOICES, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI, &payload, encode_dialogue_choices_adapter);
    return true;
}
bool tl_broadcast_dialogue_close(TransportLoopContext* ctx, const MsgDialogueClose* msg) {
    size_t i; if (!ctx || !msg) return false;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (ctx->session->players[i].connected) tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_DIALOGUE_CLOSE, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI, msg, encode_dialogue_close_adapter);
    return true;
}
bool tl_broadcast_quest_update(TransportLoopContext* ctx, const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, const char* const* texts) {
    size_t i; struct { MsgQuestUpdateHeader h; MsgQuestObjectiveRecord objs[16]; const char* texts[16]; } payload; if (!ctx || !h) return false; memset(&payload,0,sizeof(payload)); payload.h=*h; if (payload.h.objective_count>16) payload.h.objective_count=16; for (i=0;i<payload.h.objective_count;++i){ payload.objs[i]=objs[i]; payload.texts[i]=texts?texts[i]:""; }
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) if (ctx->session->players[i].connected) tl_send_single(ctx, ctx->session->players[i].connection_id, MSG_QUEST_UPDATE, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI, &payload, encode_quest_update_adapter);
    return true;
}

void tl_init(TransportLoopContext* ctx, SessionState* session) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->session = session;
    ctx->player_timeout_ticks = 0;
    if (session) {
        ctx->transport.session_id = session->session_id;
        ctx->transport.connected = true;
    }
}

void tl_set_player_timeout_ticks(TransportLoopContext* ctx, uint32_t timeout_ticks) {
    if (!ctx) return;
    ctx->player_timeout_ticks = timeout_ticks;
}

uint32_t tl_get_current_tick(const TransportLoopContext* ctx) {
    return ctx ? ctx->current_tick : 0;
}

bool tl_try_pop_dialogue_select(TransportLoopContext* ctx, PlayerId* out_player_id, MsgDialogueSelect* out_select) {
    if (!ctx || !ctx->has_pending_dialogue_select) return false;
    if (out_player_id) *out_player_id = ctx->pending_dialogue_player_id;
    if (out_select) *out_select = ctx->pending_dialogue_select;
    ctx->has_pending_dialogue_select = false;
    memset(&ctx->pending_dialogue_select, 0, sizeof(ctx->pending_dialogue_select));
    ctx->pending_dialogue_player_id = 0;
    return true;
}

void tl_clear_outputs(TransportLoopContext* ctx) { if (ctx) ctx->output_count = 0; }
size_t tl_get_output_count(const TransportLoopContext* ctx) { return ctx ? ctx->output_count : 0; }
const TransportOutput* tl_get_output(const TransportLoopContext* ctx, size_t index) { return (!ctx || index >= ctx->output_count) ? NULL : &ctx->outputs[index]; }

static void tl_load_profile_into_player(PlayerRuntimeState* slot, PlayerId player_id) {
    char profile_path[128];
    PlayerProfileData profile;
    (void)player_id;
    tl_profile_path_for_guid(slot->profile_ref.player_guid, profile_path, sizeof(profile_path));
    if (!profile_load_json(profile_path, &profile)) {
        profile_make_default(&profile, slot->profile_ref.player_guid, slot->profile_ref.display_name);
        profile_ensure_parent_dir(profile_path);
        profile_save_json(profile_path, &profile);
    }
    slot->profile_snapshot.loaded = true;
    slot->profile_snapshot.level = profile.level;
    slot->profile_snapshot.xp = profile.xp;
    slot->profile_snapshot.equipped_form_id = profile.equipped_form_id;
    slot->profile_snapshot.caps = profile.caps;
    slot->profile_snapshot.str_stat = profile.str_stat;
    slot->profile_snapshot.per_stat = profile.per_stat;
    slot->profile_snapshot.end_stat = profile.end_stat;
    slot->profile_snapshot.cha_stat = profile.cha_stat;
    slot->profile_snapshot.int_stat = profile.int_stat;
    slot->profile_snapshot.agi_stat = profile.agi_stat;
    slot->profile_snapshot.luk_stat = profile.luk_stat;
    slot->profile_snapshot.item_count = profile.item_count;
    for (uint16_t i = 0; i < profile.item_count && i < F4MP_MAX_PROFILE_ITEMS; ++i) {
        slot->profile_snapshot.items[i].form_id = profile.items[i].form_id;
        slot->profile_snapshot.items[i].count = profile.items[i].count;
        slot->profile_snapshot.items[i].equipped = profile.items[i].equipped;
    }
    slot->profile_snapshot.perk_count = profile.perk_count;
    for (uint16_t i = 0; i < profile.perk_count && i < F4MP_MAX_PROFILE_PERKS; ++i) {
        slot->profile_snapshot.perks[i].perk_form_id = profile.perks[i].perk_form_id;
        slot->profile_snapshot.perks[i].rank = profile.perks[i].rank;
    }
    slot->profile_snapshot.sex = profile.sex;
    slot->profile_snapshot.body_preset = profile.body_preset;
}

static PlayerRuntimeState* tl_find_player_by_connection(SessionState* s, uint32_t connection_id) {
    size_t i;
    if (!s) return NULL;
    for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
        if (s->players[i].connected && s->players[i].connection_id == connection_id) return &s->players[i];
    }
    return NULL;
}

static bool tl_send_pong(TransportLoopContext* ctx, uint32_t connection_id, uint32_t time_ms) {
    MsgPong msg;
    memset(&msg, 0, sizeof(msg));
    msg.time_ms = time_ms;
    return tl_send_single(ctx, connection_id, MSG_PONG, MSGF_CH0_CONTROL, &msg, encode_pong_adapter);
}

void tl_tick(TransportLoopContext* ctx) {
    size_t i;
    ActorNetId to_despawn[64];
    size_t despawn_count = 0;
    if (!ctx || !ctx->session) return;

    ctx->current_tick++;

    if (ctx->player_timeout_ticks > 0) {
        PlayerId timed_out[8];
        size_t timed_out_count = 0;
        for (i = 0; i < F4MP_MAX_PLAYERS_PER_SESSION; ++i) {
            PlayerRuntimeState* p = &ctx->session->players[i];
            if (p->connected && (ctx->current_tick - p->last_heard_tick) > ctx->player_timeout_ticks) {
                timed_out[timed_out_count++] = p->player_id;
            }
        }
        for (i = 0; i < timed_out_count; ++i) {
            tl_broadcast_player_left(ctx, timed_out[i]);
            sm_remove_player(ctx->session, timed_out[i]);
            replay_log_write("timeout_disconnect", "accepted", "{\"broadcast\":true}");
        }
    }

    ac_tick(ctx->session);
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
        if (!ctx->session->actors[i].in_use && ctx->session->actors[i].actor_net_id != 0) {
            /* unreachable with current store, kept for future */
        }
    }
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
        ActorRuntimeState* a = &ctx->session->actors[i];
        if (a->in_use && a->pending_despawn && (a->replicated_state.actor_flags & ACTF_DEAD)) {
            to_despawn[despawn_count++] = a->actor_net_id;
            if (despawn_count >= 64) break;
        }
    }
    for (i = 0; i < despawn_count; ++i) {
        tl_broadcast_actor_despawn(ctx, to_despawn[i], ADESPAWN_DEAD);
        ws_remove_actor(ctx->session, to_despawn[i]);
    }
}

bool tl_handle_envelope(TransportLoopContext* ctx, const TransportEnvelope* env) {
    PacketReader r;
    PacketHeader ph;
    MessageHeader mh;
    PlayerId resolved_player_id = 0;
    const PlayerRuntimeState* player = NULL;

    if (!ctx || !ctx->session || !env || env->length < sizeof(PacketHeader) + sizeof(MessageHeader)) return false;

    pr_init(&r, env->bytes, env->length);
    if (!decode_packet_header(&r, &ph) || !f4mp_validate_packet_header(&ph)) {
        replay_log_write("packet_rx", "error", "{\"code\":1}");
        return false;
    }
    ctx->transport.last_rx_sequence = ph.sequence;
    if (!decode_message_header(&r, &mh) || !f4mp_validate_message_header(&mh)) {
        replay_log_write("message_rx", "error", "{\"code\":3}");
        return false;
    }

    if (mh.type == MSG_HELLO) {
        MsgHello hello;
        char player_guid[F4MP_MAX_PLAYER_GUID_BYTES + 1];
        char player_name[F4MP_MAX_PLAYER_NAME_BYTES + 1];
        PlayerRuntimeState* slot;
        PlayerId candidate_id = 0;

        if (!decode_msg_hello(&r, &hello, player_guid, sizeof(player_guid), player_name, sizeof(player_name))) {
            replay_log_write("hello", "decode_fail", "{\"code\":4}");
            return false;
        }

        if (!sr_find_player_id(player_guid, &candidate_id)) {
            for (candidate_id = 1; candidate_id < F4MP_MAX_PLAYERS_PER_SESSION; ++candidate_id) {
                if (!find_player_const(ctx->session, candidate_id)) break;
            }
            if (candidate_id >= F4MP_MAX_PLAYERS_PER_SESSION) {
                replay_log_write("hello", "rejected", "{\"reason\":\"session_full\"}");
                return false;
            }
            if (!sm_join_session(ctx->session, candidate_id, env->connection_id)) return false;
            sr_bind_player(player_guid, candidate_id);
        } else {
            if (!sm_reconnect_player(ctx->session, candidate_id, env->connection_id)) return false;
        }

        slot = find_player(ctx->session, candidate_id);
        if (!slot) return false;
        strncpy(slot->profile_ref.display_name, player_name, sizeof(slot->profile_ref.display_name) - 1);
        strncpy(slot->profile_ref.player_guid, player_guid, sizeof(slot->profile_ref.player_guid) - 1);
        slot->connection_id = env->connection_id;
        slot->last_heard_tick = ctx->current_tick;
        tl_load_profile_into_player(slot, candidate_id);
        replay_log_write("hello", "accepted", "{\"assigned\":true}");
        if (!tl_send_welcome(ctx, env->connection_id, candidate_id)) return false;
        if (!tl_send_profile_snapshot(ctx, env->connection_id, slot)) return false;
        tl_send_existing_state_to_player(ctx, env->connection_id, candidate_id);
        return true;
    }

    player = tl_find_player_by_connection(ctx->session, env->connection_id);
    if (player) resolved_player_id = player->player_id;
    if (!player) {
        replay_log_write("message_rx", "error", "{\"reason\":\"unknown_connection\"}");
        return false;
    }

    switch (mh.type) {
        case MSG_PING: {
            MsgPing msg;
            PlayerRuntimeState* mutable_player = find_player(ctx->session, resolved_player_id);
            if (!decode_msg_ping(&r, &msg)) return false;
            if (mutable_player) mutable_player->last_heard_tick = ctx->current_tick;
            return tl_send_pong(ctx, env->connection_id, msg.time_ms);
        }
        case MSG_DISCONNECT: {
            MsgDisconnect msg;
            if (!decode_msg_disconnect(&r, &msg)) return false;
            (void)msg;
            { PlayerRuntimeState* mutable_player = find_player(ctx->session, resolved_player_id); if (mutable_player) mutable_player->last_heard_tick = ctx->current_tick; }
            tl_broadcast_player_left(ctx, resolved_player_id);
            sm_remove_player(ctx->session, resolved_player_id);
            replay_log_write("disconnect", "accepted", "{\"broadcast\":true}");
            return true;
        }
        case MSG_PLAYER_STATE: {
            MsgPlayerState msg;
            PlayerRuntimeState* mutable_player;
            PlayerStateRelayDecision relay_decision;
            if (!decode_msg_player_state(&r, &msg)) return false;
            if (msg.player_id != resolved_player_id) return false;
            mutable_player = find_player(ctx->session, resolved_player_id);
            if (!mutable_player) return false;
            mutable_player->last_heard_tick = ctx->current_tick;
            relay_decision = psr_evaluate(&mutable_player->relay_stats, &msg);
            if (relay_decision != PSR_ACCEPT) {
                replay_log_write("player_state", "rejected", "{\"reason\":\"stale_or_regressive\"}");
                return true;
            }
            mutable_player->replicated_state = msg;
            tl_broadcast_player_state(ctx, mutable_player);
            return true;
        }
        case MSG_INPUT_STATE: {
            MsgInputState msg;
            PlayerRuntimeState* mutable_player;
            if (!decode_msg_input_state(&r, &msg)) return false;
            if (!ac_handle_input_state(ctx->session, &msg, resolved_player_id)) return false;
            mutable_player = find_player(ctx->session, resolved_player_id);
            if (!mutable_player) return false;
            mutable_player->last_heard_tick = ctx->current_tick;
            tl_broadcast_player_state(ctx, mutable_player);
            return true;
        }
        case MSG_FIRE_INTENT: {
            MsgFireIntent msg;
            MsgDamageResult dmg;
            MsgDeathEvent death;
            bool has_death = false;
            const ActorRuntimeState* actor;
            if (!decode_msg_fire_intent(&r, &msg)) return false;
            if (msg.player_id != resolved_player_id) return false;
            { PlayerRuntimeState* mutable_player = find_player(ctx->session, resolved_player_id); if (mutable_player) mutable_player->last_heard_tick = ctx->current_tick; }
            memset(&dmg, 0, sizeof(dmg));
            memset(&death, 0, sizeof(death));
            if (!ac_handle_fire_intent(ctx->session, &msg, &dmg, &death, &has_death)) return false;
            if (dmg.victim_actor_net_id != 0) {
                tl_broadcast_damage(ctx, &dmg);
                actor = find_actor_const(ctx->session, dmg.victim_actor_net_id);
                if (actor) tl_broadcast_actor_state(ctx, actor);
                if (has_death) tl_broadcast_death(ctx, &death);
            }
            return true;
        }
        case MSG_BUILD_REQUEST: {
            MsgBuildRequest msg;
            MsgBuildResult result;
            const ObjectRuntimeState* obj;
            if (!decode_msg_build_request(&r, &msg)) return false;
            if (msg.player_id != resolved_player_id) return false;
            { PlayerRuntimeState* mutable_player = find_player(ctx->session, resolved_player_id); if (mutable_player) mutable_player->last_heard_tick = ctx->current_tick; }
            (void)ac_handle_build_request(ctx->session, &msg, &result);
            tl_send_single(ctx, env->connection_id, MSG_BUILD_RESULT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, &result, encode_build_result_adapter);
            if (result.accepted) {
                obj = find_object_const(ctx->session, result.object_net_id);
                if (obj) tl_broadcast_object_spawn(ctx, obj);
            }
            return true;
        }
        case MSG_OBJECT_MOVE_REQ: {
            MsgObjectMoveRequest msg;
            const ObjectRuntimeState* obj;
            if (!decode_msg_object_move_request(&r, &msg)) return false;
            if (msg.player_id != resolved_player_id) return false;
            if (!ac_handle_object_move(ctx->session, &msg)) return false;
            obj = find_object_const(ctx->session, msg.object_net_id);
            if (obj) tl_broadcast_object_update(ctx, obj, OUF_POSITION | OUF_ROTATION | OUF_STATE_FLAGS);
            return true;
        }
        case MSG_OBJECT_SCRAP_REQ: {
            MsgObjectScrapRequest msg;
            ObjectNetId dead_id;
            if (!decode_msg_object_scrap_request(&r, &msg)) return false;
            if (msg.player_id != resolved_player_id) return false;
            dead_id = msg.object_net_id;
            if (!ac_handle_object_scrap(ctx->session, &msg)) return false;
            tl_broadcast_object_despawn(ctx, dead_id, ODESPAWN_SCRAPPED);
            return true;
        }
        case MSG_DIALOGUE_SELECT: {
            MsgDialogueSelect msg;
            DialogueSelectionResult sr;
            if (!decode_msg_dialogue_select(&r, &msg)) return false;
            if (!dsp_apply_selection(ctx->session, resolved_player_id, &msg, &sr)) {
                replay_log_write("dialogue_select", "rejected", "{\"reason\":\"policy\"}");
                return false;
            }
            ctx->has_pending_dialogue_select = true;
            ctx->pending_dialogue_player_id = resolved_player_id;
            ctx->pending_dialogue_select = msg;
            replay_log_write("dialogue_select", "accepted", "{\"mirrored\":true}");
            return true;
        }
        default:
            replay_log_write("message_rx", "ignored", "{\"reason\":\"unsupported_type\"}");
            return false;
    }
}
