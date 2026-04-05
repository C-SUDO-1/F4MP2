#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgHello {
    uint32_t build_hash;
    uint32_t capabilities;
} MsgHello;

typedef struct MsgProfileSnapshot {
    uint32_t level;
    uint32_t xp;
    uint32_t equipped_form_id;
    uint32_t caps;
    uint8_t  str_stat;
    uint8_t  per_stat;
    uint8_t  end_stat;
    uint8_t  cha_stat;
    uint8_t  int_stat;
    uint8_t  agi_stat;
    uint8_t  luk_stat;
    uint8_t  reserved0;
} MsgProfileSnapshot;

typedef struct MsgProfileItemsHeader {
    uint16_t item_count;
    uint16_t reserved0;
} MsgProfileItemsHeader;

typedef struct MsgProfileItemRecord {
    uint32_t form_id;
    uint32_t count;
    uint8_t  equipped;
    uint8_t  reserved0[3];
} MsgProfileItemRecord;

typedef struct MsgProfilePerksHeader {
    uint16_t perk_count;
    uint16_t reserved0;
} MsgProfilePerksHeader;

typedef struct MsgProfilePerkRecord {
    uint32_t perk_form_id;
    uint8_t  rank;
    uint8_t  reserved0[3];
} MsgProfilePerkRecord;

typedef struct MsgProfileAppearance {
    uint8_t sex;
    uint8_t reserved0[3];
    uint32_t body_preset;
} MsgProfileAppearance;

typedef struct MsgWelcome {
    SessionId session_id;
    PlayerId  player_id;
    uint16_t  tick_rate;
    uint32_t  rules;
} MsgWelcome;

typedef struct MsgPing { uint32_t time_ms; } MsgPing;
typedef struct MsgPong { uint32_t time_ms; } MsgPong;
typedef struct MsgDisconnect { uint8_t reason; } MsgDisconnect;
#pragma pack(pop)

enum DisconnectReason {
    DISC_NONE = 0,
    DISC_PROTOCOL_ERROR = 1,
    DISC_VERSION_MISMATCH = 2,
    DISC_TIMEOUT = 3,
    DISC_KICKED = 4,
    DISC_SERVER_SHUTDOWN = 5
};


