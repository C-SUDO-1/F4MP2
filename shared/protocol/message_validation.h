#pragma once

#include <stdbool.h>
#include "packet_header.h"

static inline bool f4mp_validate_packet_header(const PacketHeader* h) {
    return h && h->magic == F4MP_MAGIC && h->version == F4MP_VERSION && h->message_count > 0;
}

static inline bool f4mp_validate_message_header(const MessageHeader* h) {
    return h && h->length > 0;
}

