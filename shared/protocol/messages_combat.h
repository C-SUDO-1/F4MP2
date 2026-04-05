#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgFireIntent {
    PlayerId player_id;
    uint16_t reserved0;
    uint32_t weapon_form_id;
    Vec3f    origin;
    Vec3f    direction;
    uint8_t  mode;
    uint8_t  reserved1[3];
    uint32_t fire_flags;
} MsgFireIntent;

typedef struct MsgMeleeIntent {
    PlayerId player_id;
    uint16_t reserved0;
    uint32_t weapon_form_id;
    ActorNetId target_hint_net_id;
    uint32_t melee_flags;
} MsgMeleeIntent;

typedef struct MsgDamageResult {
    uint32_t attacker_id;
    ActorNetId victim_actor_net_id;
    float      damage_amount;
    uint8_t    damage_type;
    uint8_t    reserved0;
    uint16_t   flags;
    float      health_after;
} MsgDamageResult;

typedef struct MsgDeathEvent {
    ActorNetId victim_actor_net_id;
    uint32_t   killer_id;
    uint32_t   ragdoll_seed;
} MsgDeathEvent;
#pragma pack(pop)

enum FireIntentFlags {
    FIREF_AIMING=1<<0, FIREF_HIPFIRE=1<<1, FIREF_MELEE=1<<2, FIREF_POWER_ATTACK=1<<3,
    FIREF_VATS_BLOCKED=1<<4, FIREF_AUTOMATIC_FIRE=1<<5, FIREF_EXPLOSIVE_PROJECT=1<<6, FIREF_RELOAD_STATE=1<<7
};

enum DamageResultFlags {
    DMGF_CRITICAL=1<<0, DMGF_HEADSHOT=1<<1, DMGF_LIMB_HIT=1<<2, DMGF_BLOCKED=1<<3,
    DMGF_RESISTED=1<<4, DMGF_KILLED=1<<5, DMGF_SPLASH=1<<6, DMGF_HOST_CORRECTED=1<<7
};

enum ActorStateFlags {
    ACTF_HOSTILE=1<<0, ACTF_DEAD=1<<1, ACTF_DOWNED=1<<2, ACTF_ESSENTIAL=1<<3,
    ACTF_IN_COMBAT=1<<4, ACTF_RANGED=1<<5, ACTF_MELEE_ONLY=1<<6, ACTF_SETTLEMENT_DEFENDER=1<<7
};

enum DamageType { DMG_BALLISTIC=0, DMG_ENERGY=1, DMG_MELEE=2, DMG_EXPLOSIVE=3, DMG_FIRE=4, DMG_RADIATION=5 };

