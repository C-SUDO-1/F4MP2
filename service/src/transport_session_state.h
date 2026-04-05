#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "protocol_types.h"

typedef struct TransportSessionState {
    SessionId session_id;
    uint32_t  last_rx_sequence;
    uint32_t  last_tx_sequence;
    uint32_t  last_acked_sequence;
    uint32_t  ack_bits;
    bool      connected;
} TransportSessionState;
