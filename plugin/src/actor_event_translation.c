#include "actor_event_translation.h"
#include <string.h>

bool ahe_translate_spawn(const ActorHostEvent* ev, MsgActorSpawn* out_msg) {
    if (!ev || !out_msg || ev->kind != AHE_SPAWN) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->actor_net_id = ev->actor_net_id;
    out_msg->base_form_id = ev->base_form_id;
    out_msg->position = ev->position;
    out_msg->rotation = ev->rotation;
    out_msg->actor_kind = ev->actor_kind;
    out_msg->flags = ev->actor_flags;
    return true;
}

bool ahe_translate_state(const ActorHostEvent* ev, MsgActorState* out_msg) {
    if (!ev || !out_msg || ev->kind != AHE_UPDATE) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->actor_net_id = ev->actor_net_id;
    out_msg->position = ev->position;
    out_msg->rotation = ev->rotation;
    out_msg->health_norm = ev->health_norm;
    out_msg->anim_state = ev->anim_state;
    out_msg->actor_flags = ev->actor_flags;
    out_msg->target_net_id = ev->target_net_id;
    return true;
}

bool ahe_translate_despawn(const ActorHostEvent* ev, MsgActorDespawn* out_msg) {
    if (!ev || !out_msg || ev->kind != AHE_DESPAWN) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->actor_net_id = ev->actor_net_id;
    out_msg->reason = ev->despawn_reason;
    return true;
}

bool ahe_translate_damage(const ActorHostEvent* ev, MsgDamageResult* out_msg) {
    if (!ev || !out_msg || ev->kind != AHE_DAMAGE) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->attacker_id = ev->attacker_id;
    out_msg->victim_actor_net_id = ev->actor_net_id;
    out_msg->damage_amount = ev->damage_amount;
    out_msg->damage_type = ev->damage_type;
    out_msg->flags = ev->damage_flags;
    out_msg->health_after = ev->health_after;
    return true;
}

bool ahe_translate_death(const ActorHostEvent* ev, MsgDeathEvent* out_msg) {
    if (!ev || !out_msg || ev->kind != AHE_DEATH) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->victim_actor_net_id = ev->actor_net_id;
    out_msg->killer_id = ev->killer_id;
    out_msg->ragdoll_seed = ev->ragdoll_seed;
    return true;
}
