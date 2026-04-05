#pragma once

#include <stdint.h>

enum MessageType {
    MSG_HELLO              = 0x0001,
    MSG_WELCOME            = 0x0002,
    MSG_PING               = 0x0003,
    MSG_PONG               = 0x0004,
    MSG_DISCONNECT         = 0x0005,
    MSG_PROFILE_SNAPSHOT   = 0x0006,
    MSG_PROFILE_ITEMS      = 0x0007,
    MSG_PROFILE_PERKS      = 0x0008,
    MSG_PROFILE_APPEARANCE = 0x0009,

    MSG_INPUT_STATE        = 0x0101,
    MSG_PLAYER_STATE       = 0x0102,
    MSG_PLAYER_LEFT        = 0x0105,
    MSG_INTEREST_SET       = 0x0103,
    MSG_BUBBLE_VIOLATION   = 0x0104,

    MSG_ACTOR_SPAWN        = 0x0201,
    MSG_ACTOR_STATE        = 0x0202,
    MSG_ACTOR_DESPAWN      = 0x0203,

    MSG_FIRE_INTENT        = 0x0301,
    MSG_MELEE_INTENT       = 0x0302,
    MSG_DAMAGE_RESULT      = 0x0303,
    MSG_DEATH_EVENT        = 0x0304,

    MSG_BUILD_REQUEST      = 0x0401,
    MSG_BUILD_RESULT       = 0x0402,
    MSG_OBJECT_SPAWN       = 0x0403,
    MSG_OBJECT_MOVE_REQ    = 0x0404,
    MSG_OBJECT_SCRAP_REQ   = 0x0405,
    MSG_OBJECT_UPDATE      = 0x0406,
    MSG_OBJECT_DESPAWN     = 0x0407,

    MSG_DIALOGUE_OPEN      = 0x0501,
    MSG_DIALOGUE_LINE      = 0x0502,
    MSG_DIALOGUE_CHOICES   = 0x0503,
    MSG_DIALOGUE_SELECT    = 0x0504,
    MSG_DIALOGUE_CLOSE     = 0x0505,

    MSG_QUEST_UPDATE       = 0x0601
};

enum MessageFlags {
    MSGF_NONE        = 0,
    MSGF_RELIABLE    = 1 << 0,
    MSGF_ORDERED     = 1 << 1,

    MSGF_CH0_CONTROL = 0 << 8,
    MSGF_CH1_WORLD   = 1 << 8,
    MSGF_CH2_COMBAT  = 2 << 8,
    MSGF_CH3_WORK    = 3 << 8,
    MSGF_CH4_UI      = 4 << 8
};

