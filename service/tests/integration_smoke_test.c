#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "session_manager.h"
#include "session_rules.h"
#include "transport_loop.h"
#include "packet_codec.h"
#include "message_ids.h"
#include "world_state_store.h"

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

static size_t build_build_request_packet(uint8_t* out, size_t cap, PlayerId pid, uint32_t settlement_id, uint32_t form_id) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; MsgBuildRequest req = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = MSG_BUILD_REQUEST; mh.flags = MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK;
    req.request_id = 1; req.player_id = pid; req.settlement_id = settlement_id; req.form_id = form_id;
    pw_init(&w, out, cap); assert(encode_packet_header(&w, &ph)); assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w); assert(encode_msg_build_request(&w, &req));
    return finalize_packet(&w, payload_start);
}

static size_t build_fire_packet(uint8_t* out, size_t cap, PlayerId pid, uint32_t weapon_form_id) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; MsgFireIntent req = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = MSG_FIRE_INTENT; mh.flags = MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH2_COMBAT;
    req.player_id = pid; req.weapon_form_id = weapon_form_id; req.direction.x = 1.0f; req.fire_flags = FIREF_HIPFIRE;
    pw_init(&w, out, cap); assert(encode_packet_header(&w, &ph)); assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w); assert(encode_msg_fire_intent(&w, &req));
    return finalize_packet(&w, payload_start);
}


int main(void) {
    SessionState* s; TransportLoopContext tl; TransportEnvelope env = {0}; size_t outputs; int saw_damage = 0, saw_actor_spawn = 0;
    sm_init();
    s = sm_create_session(1, RULE_BUBBLE_ONLY | RULE_HOST_AUTH_COMBAT | RULE_HOST_AUTH_WORKSHOP | RULE_GUEST_PROFILE_ENABLED, 20, 0);
    assert(s);
    assert(ws_alloc_settlement(s, 1001, (Vec3f){0,0,0}, 2048.0f, 16));
    assert(sm_spawn_default_hostile(s, 0x90000001u, (Vec3f){128.0f, 0.0f, 0.0f}));
    tl_init(&tl, s);

    env.connection_id = 101;
    env.length = build_hello_packet(env.bytes, sizeof(env.bytes), "guid-guest-001", "guest");
    assert(tl_handle_envelope(&tl, &env));
    outputs = tl_get_output_count(&tl);
    assert(outputs >= 2);
    tl_clear_outputs(&tl);

    env.connection_id = 101;
    env.length = build_build_request_packet(env.bytes, sizeof(env.bytes), 1, 1001, 0xAAAAAAAAu);
    assert(tl_handle_envelope(&tl, &env));
    outputs = tl_get_output_count(&tl);
    assert(outputs >= 2);
    tl_clear_outputs(&tl);


    env.connection_id = 101;
    env.length = build_fire_packet(env.bytes, sizeof(env.bytes), 1, 0x12345678u);
    assert(tl_handle_envelope(&tl, &env));
    outputs = tl_get_output_count(&tl);
    assert(outputs >= 2);
    for (size_t i = 0; i < outputs; ++i) {
        const TransportOutput* out = tl_get_output(&tl, i);
        PacketReader r; PacketHeader ph; MessageHeader mh;
        pr_init(&r, out->bytes, out->length);
        assert(decode_packet_header(&r, &ph));
        assert(decode_message_header(&r, &mh));
        if (mh.type == MSG_DAMAGE_RESULT) saw_damage = 1;
    }
    assert(saw_damage);

    printf("integration_smoke_test passed\n");
    return 0;
}
