#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "protocol_types.h"
#include "messages_player.h"
#include "messages_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ProxyPlayerSpawnSpec {
    PlayerId player_id;
    NetId    proxy_net_id;
    uint32_t proxy_base_form_id;
    Vec3f    position;
    Rot3f    rotation;
    uint32_t equipped_form_id;
} ProxyPlayerSpawnSpec;

typedef struct ProxyPlayerUpdateSpec {
    PlayerId  player_id;
    Vec3f     position;
    Rot3f     rotation;
    Vec3f     velocity;
    uint8_t   stance;
    uint16_t  anim_state;
    uint32_t  equipped_form_id;
} ProxyPlayerUpdateSpec;

typedef struct ProxyActorSpawnSpec {
    ActorNetId actor_net_id;
    uint32_t   base_form_id;
    Vec3f      position;
    Rot3f      rotation;
    uint8_t    actor_kind;
    uint16_t   flags;
} ProxyActorSpawnSpec;

typedef struct ProxyActorUpdateSpec {
    ActorNetId actor_net_id;
    Vec3f      position;
    Rot3f      rotation;
    float      health_norm;
    uint16_t   anim_state;
    uint16_t   actor_flags;
    NetId      target_net_id;
} ProxyActorUpdateSpec;

bool proxy_spawn_player(const ProxyPlayerSpawnSpec* spec);
bool proxy_update_player(const ProxyPlayerUpdateSpec* spec);
bool proxy_despawn_player(PlayerId player_id);
bool proxy_spawn_actor(const ProxyActorSpawnSpec* spec);
bool proxy_update_actor(const ProxyActorUpdateSpec* spec);
bool proxy_despawn_actor(ActorNetId actor_net_id);

#ifdef __cplusplus
}
#endif
