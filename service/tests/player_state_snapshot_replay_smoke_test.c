#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "session_manager.h"
#include "session_rules.h"
#include "transport_loop.h"
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

static size_t build_player_state_packet(uint8_t* out, size_t cap, PlayerId pid, float x, float y, float yaw) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; MsgPlayerState ps = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = MSG_PLAYER_STATE; mh.flags = MSGF_CH1_WORLD;
    ps.player_id = pid; ps.position.x = x; ps.position.y = y; ps.rotation.yaw = yaw; ps.stance = STANCE_RUN;
    pw_init(&w, out, cap);
    assert(encode_packet_header(&w, &ph)); assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w);
    assert(encode_msg_player_state(&w, &ps));
    return finalize_packet(&w, payload_start);
}

int main(void) {
    SessionState* s; TransportLoopContext tl; TransportEnvelope env = {0};
    int saw_replayed_player = 0;
    sm_init();
    s = sm_create_session(1, RULE_BUBBLE_ONLY | RULE_GUEST_PROFILE_ENABLED, 20, 0);
    assert(s);
    tl_init(&tl, s);

    env.connection_id = 101;
    env.length = build_hello_packet(env.bytes, sizeof(env.bytes), "guid-a", "A");
    assert(tl_handle_envelope(&tl, &env));
    tl_clear_outputs(&tl);

    env.connection_id = 101;
    env.length = build_player_state_packet(env.bytes, sizeof(env.bytes), 1, 12.0f, 34.0f, 90.0f);
    assert(tl_handle_envelope(&tl, &env));
    tl_clear_outputs(&tl);

    env.connection_id = 102;
    env.length = build_hello_packet(env.bytes, sizeof(env.bytes), "guid-b", "B");
    assert(tl_handle_envelope(&tl, &env));

    for (size_t i = 0; i < tl_get_output_count(&tl); ++i) {
        const TransportOutput* out = tl_get_output(&tl, i);
        if (out->connection_id != 102) continue;
        PacketReader r; PacketHeader ph; MessageHeader mh;
        pr_init(&r, out->bytes, out->length);
        assert(decode_packet_header(&r, &ph));
        assert(decode_message_header(&r, &mh));
        if (mh.type == MSG_PLAYER_STATE) {
            MsgPlayerState ps;
            assert(decode_msg_player_state(&r, &ps));
            if (ps.player_id == 1 && ps.position.x == 12.0f && ps.position.y == 34.0f && ps.rotation.yaw == 90.0f) {
                saw_replayed_player = 1;
            }
        }
    }

    assert(saw_replayed_player);
    return 0;
}
