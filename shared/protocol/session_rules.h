#pragma once

#include <stdint.h>

enum SessionRules {
    RULE_NONE                    = 0,
    RULE_BUBBLE_ONLY             = 1u << 0,
    RULE_NO_INTERIORS            = 1u << 1,
    RULE_HOST_AUTH_COMBAT        = 1u << 2,
    RULE_HOST_AUTH_WORKSHOP      = 1u << 3,
    RULE_GUEST_PROFILE_ENABLED   = 1u << 4,
    RULE_DIALOGUE_MIRROR_ENABLED = 1u << 5,
    RULE_QUEST_MIRROR_ENABLED    = 1u << 6
};

