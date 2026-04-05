#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_actor.h"
#include "messages_combat.h"

typedef enum ActorHostEventKind {
    AHE_SPAWN = 0,
    AHE_UPDATE = 1,
    AHE_DESPAWN = 2,
    AHE_DAMAGE = 3,
    AHE_DEATH = 4
} ActorHostEventKind;

typedef struct ActorHostEvent {
    ActorHostEventKind kind;
    ActorNetId actor_net_id;
    uint32_t base_form_id;
    Vec3f position;
    Rot3f rotation;
    uint8_t actor_kind;
    uint16_t actor_flags;
    float health_norm;
    uint16_t anim_state;
    NetId target_net_id;
    uint8_t despawn_reason;
    uint32_t attacker_id;
    float damage_amount;
    uint8_t damage_type;
    uint16_t damage_flags;
    float health_after;
    uint32_t killer_id;
    uint32_t ragdoll_seed;
} ActorHostEvent;

bool ahe_translate_spawn(const ActorHostEvent* ev, MsgActorSpawn* out_msg);
bool ahe_translate_state(const ActorHostEvent* ev, MsgActorState* out_msg);
bool ahe_translate_despawn(const ActorHostEvent* ev, MsgActorDespawn* out_msg);
bool ahe_translate_damage(const ActorHostEvent* ev, MsgDamageResult* out_msg);
bool ahe_translate_death(const ActorHostEvent* ev, MsgDeathEvent* out_msg);
