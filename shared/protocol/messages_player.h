#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgInputState {
    uint32_t client_tick;
    int8_t   move_x;
    int8_t   move_y;
    int16_t  look_yaw;
    int16_t  look_pitch;
    uint32_t buttons;
    uint32_t equipped_form_id;
} MsgInputState;

typedef struct MsgPlayerState {
    PlayerId player_id;
    uint16_t reserved;
    Vec3f    position;
    Rot3f    rotation;
    Vec3f    velocity;
    uint8_t  stance;
    uint8_t  reserved2[3];
    uint16_t anim_state;
    uint16_t reserved3;
    uint32_t equipped_form_id;
    uint32_t state_sequence;
    uint32_t observed_tick;
} MsgPlayerState;

typedef struct MsgPlayerLeft {
    PlayerId player_id;
    uint16_t reserved;
} MsgPlayerLeft;

typedef struct MsgInterestSet {
    Vec3f    bubble_center;
    float    bubble_radius;
    uint16_t cell_count;
    uint16_t reserved;
} MsgInterestSet;

typedef struct MsgBubbleViolation {
    PlayerId player_id;
    uint8_t  mode;
    uint8_t  reserved;
    Vec3f    safe_position;
} MsgBubbleViolation;
#pragma pack(pop)

enum InputButtons {
    BTN_JUMP         = 1u << 0,
    BTN_SPRINT       = 1u << 1,
    BTN_CROUCH       = 1u << 2,
    BTN_FIRE         = 1u << 3,
    BTN_AIM          = 1u << 4,
    BTN_USE          = 1u << 5,
    BTN_RELOAD       = 1u << 6,
    BTN_MELEE        = 1u << 7,
    BTN_POWER_ATTACK = 1u << 8
};

enum PlayerStance {
    STANCE_IDLE=0, STANCE_WALK=1, STANCE_RUN=2, STANCE_SPRINT=3,
    STANCE_CROUCH=4, STANCE_AIM=5, STANCE_JUMP=6, STANCE_FALL=7
};

enum BubbleViolationMode {
    BUBBLE_WARN=0, BUBBLE_CLAMP=1, BUBBLE_SNAP=2, BUBBLE_RESPAWN=3
};

