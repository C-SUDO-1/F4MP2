#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "session_manager.h"
#include "session_rules.h"
#include "transport_loop.h"
#include "service_indices.h"
#include "packet_codec.h"
#include "message_ids.h"

static size_t finalize_packet(PacketWriter* w, size_t payload_start) {
    uint16_t payload_len = (uint16_t)(pw_bytes_written(w) - payload_start);
    memcpy(w->buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return pw_bytes_written(w);
}

static size_t build_hello_packet(uint8_t* out, size_t cap, const char* guid, const char* name) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; MsgHello hello = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = MSG_HELLO; mh.flags = MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL;
    pw_init(&w, out, cap);
    assert(encode_packet_header(&w, &ph)); assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w);
    assert(encode_msg_hello(&w, &hello, guid, name));
    return finalize_packet(&w, payload_start);
}

int main(void) {
    SessionState* s; TransportLoopContext tl; TransportEnvelope env = {0};
    sm_init();
    s = sm_create_session(1, RULE_BUBBLE_ONLY, 20, 0);
    assert(s);
    tl_init(&tl, s);
    tl_set_player_timeout_ticks(&tl, 3);

    env.connection_id = 101;
    env.length = build_hello_packet(env.bytes, sizeof(env.bytes), "guid-timeout-1", "timeout");
    assert(tl_handle_envelope(&tl, &env));
    assert(find_player_const(s, 1) != NULL);
    tl_clear_outputs(&tl);

    tl_tick(&tl);
    tl_tick(&tl);
    tl_tick(&tl);
    assert(find_player_const(s, 1) != NULL);
    tl_tick(&tl);
    assert(find_player_const(s, 1) == NULL);
    assert(tl_get_output_count(&tl) >= 1);

    {
        const TransportOutput* out = tl_get_output(&tl, 0);
        PacketReader r; PacketHeader ph; MessageHeader mh; MsgPlayerLeft left = {0};
        pr_init(&r, out->bytes, out->length);
        assert(decode_packet_header(&r, &ph));
        assert(decode_message_header(&r, &mh));
        assert(mh.type == MSG_PLAYER_LEFT);
        assert(decode_msg_player_left(&r, &left));
        assert(left.player_id == 1);
    }
    return 0;
}
