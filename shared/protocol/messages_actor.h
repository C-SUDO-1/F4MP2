#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgActorSpawn {
    ActorNetId actor_net_id;
    uint32_t   base_form_id;
    Vec3f      position;
    Rot3f      rotation;
    uint8_t    actor_kind;
    uint8_t    reserved0;
    uint16_t   flags;
} MsgActorSpawn;

typedef struct MsgActorState {
    ActorNetId actor_net_id;
    Vec3f      position;
    Rot3f      rotation;
    float      health_norm;
    uint16_t   anim_state;
    uint16_t   actor_flags;
    NetId      target_net_id;
} MsgActorState;

typedef struct MsgActorDespawn {
    ActorNetId actor_net_id;
    uint8_t    reason;
    uint8_t    reserved0[3];
} MsgActorDespawn;
#pragma pack(pop)

enum ActorKind { ACTOR_KIND_NPC=0, ACTOR_KIND_CREATURE=1, ACTOR_KIND_TURRET=2, ACTOR_KIND_PROXY=3 };
enum ActorDespawnReason { ADESPAWN_OUT_OF_SCOPE=0, ADESPAWN_CLEANUP=1, ADESPAWN_DISABLED=2, ADESPAWN_DEAD=3 };

