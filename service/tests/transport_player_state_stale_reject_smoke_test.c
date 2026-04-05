#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "session_manager.h"
#include "session_rules.h"
#include "transport_loop.h"
#include "packet_codec.h"
#include "service_indices.h"
#include "message_ids.h"
#include "player_state_relay_report.h"

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

static size_t build_player_state_packet(uint8_t* out, size_t cap, PlayerId pid, float x, uint32_t seq, uint32_t tick) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; MsgPlayerState ps = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = MSG_PLAYER_STATE; mh.flags = MSGF_CH1_WORLD;
    ps.player_id = pid; ps.position.x = x; ps.state_sequence = seq; ps.observed_tick = tick;
    pw_init(&w, out, cap);
    assert(encode_packet_header(&w, &ph)); assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w);
    assert(encode_msg_player_state(&w, &ps));
    return finalize_packet(&w, payload_start);
}

int main(void) {
    SessionState* s; TransportLoopContext tl; TransportEnvelope env = {0};
    const PlayerStateRelayStats* stats;
    sm_init();
    s = sm_create_session(1, RULE_BUBBLE_ONLY, 20, 0);
    assert(s);
    tl_init(&tl, s);

    env.connection_id = 101;
    env.length = build_hello_packet(env.bytes, sizeof(env.bytes), "guid-a", "A");
    assert(tl_handle_envelope(&tl, &env));
    tl_clear_outputs(&tl);

    env.length = build_player_state_packet(env.bytes, sizeof(env.bytes), 1, 12.0f, 2, 2);
    assert(tl_handle_envelope(&tl, &env));
    assert(find_player_const(s,1)->replicated_state.position.x == 12.0f);

    env.length = build_player_state_packet(env.bytes, sizeof(env.bytes), 1, 3.0f, 1, 1);
    assert(tl_handle_envelope(&tl, &env));
    assert(find_player_const(s,1)->replicated_state.position.x == 12.0f);
    stats = psrr_get_player_stats(s, 1);
    assert(stats);
    assert(stats->accepted_count == 1);
    assert(stats->rejected_stale_count == 1 || stats->rejected_regressive_tick_count == 1);
    return 0;
}
