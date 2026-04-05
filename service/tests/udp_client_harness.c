#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>

#include "socket_transport.h"
#include "packet_codec.h"
#include "message_ids.h"
#include "messages_control.h"
#include "messages_player.h"
#include "messages_workshop.h"
#include "messages_combat.h"

static size_t finalize_packet(PacketWriter* w, size_t payload_start) {
    uint16_t payload_len = (uint16_t)(pw_bytes_written(w) - payload_start);
    memcpy(w->buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return pw_bytes_written(w);
}

static size_t build_one(uint8_t* out, size_t cap, uint16_t type, uint16_t flags, bool (*enc)(PacketWriter*, const void*), const void* msg) {
    PacketWriter w; PacketHeader ph = {0}; MessageHeader mh = {0}; size_t payload_start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = type; mh.flags = flags;
    pw_init(&w, out, cap);
    assert(encode_packet_header(&w, &ph));
    assert(encode_message_header(&w, &mh));
    payload_start = pw_bytes_written(&w);
    assert(enc(&w, msg));
    return finalize_packet(&w, payload_start);
}

static bool enc_hello_wrap(PacketWriter* w, const void* m) {
    const MsgHello* h = (const MsgHello*)m;
    return encode_msg_hello(w, h, "guid-harness-001", "harness");
}
static bool enc_build_wrap(PacketWriter* w, const void* m) { return encode_msg_build_request(w, (const MsgBuildRequest*)m); }
static bool enc_fire_wrap(PacketWriter* w, const void* m) { return encode_msg_fire_intent(w, (const MsgFireIntent*)m); }

static int recv_packet(SocketTransport* t, uint8_t* rx, size_t* rx_len, SocketEndpoint* ep, int timeout_ms) {
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(t->fd, &rfds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    if (select(t->fd + 1, &rfds, NULL, NULL, &tv) <= 0) return 0;
    return st_recv(t, rx, 1400, rx_len, ep) ? 1 : 0;
}

int main(int argc, char** argv) {
    SocketTransport t;
    SocketEndpoint server;
    uint8_t tx[1400], rx[1400];
    size_t tx_len, rx_len;
    int i, assert_mode = 0;
    uint32_t port = 7777;
    PlayerId pid = 0;
    int got_welcome = 0, got_profile = 0, got_build_result = 0, got_object_spawn = 0;
    int got_actor_spawn = 0, got_damage = 0, got_death = 0;

    if (argc > 1) port = (uint32_t)atoi(argv[1]);
    if (argc > 2 && strcmp(argv[2], "--assert-basic") == 0) assert_mode = 1;

    if (!st_open_udp(&t, 0)) {
        fprintf(stderr, "failed to open UDP client socket\n");
        return 1;
    }
    memset(&server, 0, sizeof(server));
    server.addr.sin_family = AF_INET;
    server.addr.sin_port = htons((uint16_t)port);
    server.addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server.addr_len = sizeof(server.addr);

    {
        MsgHello h = {0};
        tx_len = build_one(tx, sizeof(tx), MSG_HELLO, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, enc_hello_wrap, &h);
        st_send(&t, tx, tx_len, &server);
    }

    for (i = 0; i < 20 && recv_packet(&t, rx, &rx_len, &server, 200); ++i) {
        PacketReader r;
        PacketHeader ph;
        MessageHeader mh;
        pr_init(&r, rx, rx_len);
        assert(decode_packet_header(&r, &ph));
        assert(decode_message_header(&r, &mh));
        if (mh.type == MSG_WELCOME) {
            MsgWelcome w;
            assert(decode_msg_welcome(&r, &w));
            pid = w.player_id;
            got_welcome = 1;
        } else if (mh.type == MSG_PROFILE_SNAPSHOT) {
            MsgProfileSnapshot ps;
            assert(decode_msg_profile_snapshot(&r, &ps));
            got_profile = 1;
        }
        if (got_welcome && got_profile) break;
    }
    if (!pid) {
        fprintf(stderr, "join failed\n");
        st_close(&t);
        return 1;
    }

    {
        MsgBuildRequest req = {0};
        req.request_id = 1;
        req.player_id = pid;
        req.settlement_id = 1001;
        req.form_id = 0xAAAAAAAAu;
        tx_len = build_one(tx, sizeof(tx), MSG_BUILD_REQUEST, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH3_WORK, enc_build_wrap, &req);
        st_send(&t, tx, tx_len, &server);
    }
    for (i = 0; i < 6; ++i) {
        MsgFireIntent req = {0};
        req.player_id = pid;
        req.weapon_form_id = 0x12345678u;
        req.direction.x = 1.0f;
        req.fire_flags = FIREF_HIPFIRE;
        tx_len = build_one(tx, sizeof(tx), MSG_FIRE_INTENT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH2_COMBAT, enc_fire_wrap, &req);
        st_send(&t, tx, tx_len, &server);
    }

    for (i = 0; i < 80 && recv_packet(&t, rx, &rx_len, &server, 100); ++i) {
        PacketReader r;
        PacketHeader ph;
        MessageHeader mh;
        pr_init(&r, rx, rx_len);
        assert(decode_packet_header(&r, &ph));
        assert(decode_message_header(&r, &mh));
        if (mh.type == MSG_BUILD_RESULT) got_build_result = 1;
        else if (mh.type == MSG_OBJECT_SPAWN) got_object_spawn = 1;
        else if (mh.type == MSG_ACTOR_SPAWN) got_actor_spawn = 1;
        else if (mh.type == MSG_DAMAGE_RESULT) got_damage = 1;
        else if (mh.type == MSG_DEATH_EVENT) got_death = 1;
        printf("rx message type=0x%04x len=%zu\n", mh.type, rx_len);
        if (got_build_result && got_object_spawn && got_actor_spawn && got_damage && got_death) break;
    }

    st_close(&t);
    if (assert_mode) {
        if (!(got_welcome && got_profile && got_build_result && got_object_spawn && got_actor_spawn && got_damage && got_death)) {
            fprintf(stderr, "missing expected sequence: w=%d p=%d br=%d os=%d as=%d dmg=%d death=%d\n",
                    got_welcome, got_profile, got_build_result, got_object_spawn, got_actor_spawn, got_damage, got_death);
            return 2;
        }
    }
    return 0;
}
