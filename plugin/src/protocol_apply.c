#include "protocol_apply.h"

#include <string.h>

#include "packet_codec.h"
#include "message_validation.h"
#include "message_ids.h"
#include "proxy_runtime.h"
#include "remote_proxy_manager.h"



bool pa_apply_player_state(const MsgPlayerState* msg) {
    if (!msg) return false;
    return rpm_apply_remote_player_state(msg);
}

bool pa_apply_actor_spawn(const MsgActorSpawn* msg) {
    ProxyActorSpawnSpec sp;
    memset(&sp,0,sizeof(sp));
    if (!msg) return false;
    sp.actor_net_id = msg->actor_net_id; sp.base_form_id = msg->base_form_id; sp.position = msg->position; sp.rotation = msg->rotation; sp.actor_kind = msg->actor_kind; sp.flags = msg->flags;
    return proxy_spawn_actor(&sp);
}

bool pa_apply_actor_state(const MsgActorState* msg) {
    ProxyActorUpdateSpec up;
    if (!msg) return false;
    memset(&up,0,sizeof(up));
    if (!proxy_runtime_get_actor(msg->actor_net_id)) {
        ProxyActorSpawnSpec sp; memset(&sp,0,sizeof(sp)); sp.actor_net_id = msg->actor_net_id; sp.position = msg->position; sp.rotation = msg->rotation;
        if (!proxy_spawn_actor(&sp)) return false;
    }
    up.actor_net_id = msg->actor_net_id; up.position = msg->position; up.rotation = msg->rotation; up.health_norm = msg->health_norm; up.anim_state = msg->anim_state; up.actor_flags = msg->actor_flags; up.target_net_id = msg->target_net_id;
    return proxy_update_actor(&up);
}

bool pa_apply_actor_despawn(const MsgActorDespawn* msg) { if (!msg) return false; return proxy_despawn_actor(msg->actor_net_id); }
bool pa_apply_object_spawn(const MsgObjectSpawn* msg) { if (!msg) return false; return proxy_spawn_object(msg); }
bool pa_apply_object_update(const MsgObjectUpdate* msg) { if (!msg) return false; return proxy_update_object(msg); }
bool pa_apply_object_despawn(const MsgObjectDespawn* msg) { if (!msg) return false; return proxy_despawn_object(msg->object_net_id); }
bool pa_apply_damage_result(const MsgDamageResult* msg) { if (!msg) return false; proxy_set_damage_result(msg); return true; }
bool pa_apply_death_event(const MsgDeathEvent* msg) { if (!msg) return false; proxy_set_death_event(msg); return true; }
bool pa_apply_dialogue_open(const MsgDialogueOpen* msg) { if (!msg) return false; proxy_set_dialogue_open(msg); return true; }
bool pa_apply_dialogue_line(const MsgDialogueLine* msg, const char* text) { if (!msg) return false; proxy_set_dialogue_line(msg, text); return true; }
bool pa_apply_dialogue_choices(const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, char texts[][256]) { if (!h) return false; proxy_set_dialogue_choices(h, choices, texts); return true; }
bool pa_apply_dialogue_close(const MsgDialogueClose* msg) { if (!msg) return false; proxy_set_dialogue_close(msg); return true; }
bool pa_apply_quest_update(const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, char texts[][256]) { if (!h) return false; proxy_set_quest_update(h, objs, texts); return true; }

bool plugin_apply_packet(const uint8_t* bytes, size_t length) {
    PacketReader r;
    PacketHeader ph;
    MessageHeader mh;
    if (!bytes || length < sizeof(PacketHeader) + sizeof(MessageHeader)) return false;
    pr_init(&r, bytes, length);
    if (!decode_packet_header(&r, &ph) || !f4mp_validate_packet_header(&ph)) return false;
    if (!decode_message_header(&r, &mh) || !f4mp_validate_message_header(&mh)) return false;
    switch (mh.type) {
        case MSG_PROFILE_SNAPSHOT: {
            MsgProfileSnapshot msg;
            if (!decode_msg_profile_snapshot(&r, &msg)) return false;
            proxy_set_profile_snapshot(&msg);
            return true;
        }
        case MSG_PROFILE_ITEMS: {
            MsgProfileItemsHeader h; MsgProfileItemRecord items[16];
            memset(&h,0,sizeof(h)); memset(items,0,sizeof(items));
            if (!decode_msg_profile_items(&r, &h, items, 16)) return false;
            proxy_set_profile_items(&h, items);
            return true;
        }
        case MSG_PROFILE_PERKS: {
            MsgProfilePerksHeader h; MsgProfilePerkRecord perks[16];
            memset(&h,0,sizeof(h)); memset(perks,0,sizeof(perks));
            if (!decode_msg_profile_perks(&r, &h, perks, 16)) return false;
            proxy_set_profile_perks(&h, perks);
            return true;
        }
        case MSG_PROFILE_APPEARANCE: {
            MsgProfileAppearance msg;
            if (!decode_msg_profile_appearance(&r, &msg)) return false;
            proxy_set_profile_appearance(&msg);
            return true;
        }
        case MSG_PLAYER_STATE: {
            MsgPlayerState msg;
            if (!decode_msg_player_state(&r, &msg)) return false;
            return pa_apply_player_state(&msg);
        }
        case MSG_PLAYER_LEFT: {
            MsgPlayerLeft msg;
            if (!decode_msg_player_left(&r, &msg)) return false;
            (void)rpm_remove_remote_player(msg.player_id);
            return true;
        }
        case MSG_ACTOR_SPAWN: {
            MsgActorSpawn msg;
            if (!decode_msg_actor_spawn(&r, &msg)) return false;
            return pa_apply_actor_spawn(&msg);
        }
        case MSG_ACTOR_STATE: {
            MsgActorState msg;
            if (!decode_msg_actor_state(&r, &msg)) return false;
            return pa_apply_actor_state(&msg);
        }
        case MSG_ACTOR_DESPAWN: {
            MsgActorDespawn msg; if (!decode_msg_actor_despawn(&r, &msg)) return false; return pa_apply_actor_despawn(&msg);
        }
        case MSG_DAMAGE_RESULT: {
            MsgDamageResult msg; if (!decode_msg_damage_result(&r, &msg)) return false; return pa_apply_damage_result(&msg);
        }
        case MSG_DEATH_EVENT: {
            MsgDeathEvent msg; if (!decode_msg_death_event(&r, &msg)) return false; return pa_apply_death_event(&msg);
        }

        case MSG_DIALOGUE_OPEN: {
            MsgDialogueOpen msg;
            if (!decode_msg_dialogue_open(&r, &msg)) return false;
            return pa_apply_dialogue_open(&msg);
        }
        case MSG_DIALOGUE_LINE: {
            MsgDialogueLine msg; char text_buf[256];
            if (!decode_msg_dialogue_line(&r, &msg, text_buf, sizeof(text_buf))) return false;
            return pa_apply_dialogue_line(&msg, text_buf);
        }
        case MSG_DIALOGUE_CHOICES: {
            MsgDialogueChoicesHeader h; MsgDialogueChoiceRecord choices[12]; char texts[12][256];
            memset(&h,0,sizeof(h)); memset(choices,0,sizeof(choices)); memset(texts,0,sizeof(texts));
            if (!decode_msg_dialogue_choices(&r, &h, choices, texts, 12)) return false;
            return pa_apply_dialogue_choices(&h, choices, texts);
        }
        case MSG_DIALOGUE_CLOSE: {
            MsgDialogueClose msg;
            if (!decode_msg_dialogue_close(&r, &msg)) return false;
            return pa_apply_dialogue_close(&msg);
        }
        case MSG_QUEST_UPDATE: {
            MsgQuestUpdateHeader h; MsgQuestObjectiveRecord objs[16]; char texts[16][256];
            memset(&h,0,sizeof(h)); memset(objs,0,sizeof(objs)); memset(texts,0,sizeof(texts));
            if (!decode_msg_quest_update(&r, &h, objs, texts, 16)) return false;
            return pa_apply_quest_update(&h, objs, texts);
        }

        case MSG_OBJECT_SPAWN: {
            MsgObjectSpawn msg; if (!decode_msg_object_spawn(&r, &msg)) return false; return pa_apply_object_spawn(&msg);
        }
        case MSG_OBJECT_UPDATE: {
            MsgObjectUpdate msg; if (!decode_msg_object_update(&r, &msg)) return false; return pa_apply_object_update(&msg);
        }
        case MSG_OBJECT_DESPAWN: {
            MsgObjectDespawn msg; if (!decode_msg_object_despawn(&r, &msg)) return false; return pa_apply_object_despawn(&msg);
        }
        default:
            return false;
    }
}
