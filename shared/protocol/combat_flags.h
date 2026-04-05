#pragma once

#include <stdint.h>

enum FireIntentFlags {
    FIREF_AIMING             = 1u << 0,
    FIREF_HIPFIRE            = 1u << 1,
    FIREF_MELEE              = 1u << 2,
    FIREF_POWER_ATTACK       = 1u << 3,
    FIREF_VATS_BLOCKED       = 1u << 4,
    FIREF_AUTOMATIC_FIRE     = 1u << 5,
    FIREF_EXPLOSIVE_PROJECT  = 1u << 6,
    FIREF_RELOAD_STATE       = 1u << 7
};

enum DamageResultFlags {
    DMGF_CRITICAL            = 1 << 0,
    DMGF_HEADSHOT            = 1 << 1,
    DMGF_LIMB_HIT            = 1 << 2,
    DMGF_BLOCKED             = 1 << 3,
    DMGF_RESISTED            = 1 << 4,
    DMGF_KILLED              = 1 << 5,
    DMGF_SPLASH              = 1 << 6,
    DMGF_HOST_CORRECTED      = 1 << 7
};

enum ActorStateFlags {
    ACTF_HOSTILE             = 1 << 0,
    ACTF_DEAD                = 1 << 1,
    ACTF_DOWNED              = 1 << 2,
    ACTF_ESSENTIAL           = 1 << 3,
    ACTF_IN_COMBAT           = 1 << 4,
    ACTF_RANGED              = 1 << 5,
    ACTF_MELEE_ONLY          = 1 << 6,
    ACTF_SETTLEMENT_DEFENDER = 1 << 7
};

enum DamageType {
    DMG_BALLISTIC = 0,
    DMG_ENERGY    = 1,
    DMG_MELEE     = 2,
    DMG_EXPLOSIVE = 3,
    DMG_FIRE      = 4,
    DMG_RADIATION = 5
};

