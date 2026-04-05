#pragma once

#include <stdint.h>

#define F4MP_MAGIC   0x504D3446u
#define F4MP_VERSION 0x0001u

#pragma pack(push, 1)
typedef struct PacketHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t session_id;
    uint32_t sequence;
    uint32_t ack;
    uint32_t ack_bits;
    uint8_t  message_count;
    uint8_t  reserved[3];
} PacketHeader;

typedef struct MessageHeader {
    uint16_t type;
    uint16_t flags;
    uint16_t length;
} MessageHeader;
#pragma pack(pop)

enum PacketFlags {
    PKTF_NONE        = 0,
    PKTF_COMPRESSED  = 1 << 0,
    PKTF_ENCRYPTED   = 1 << 1
};

