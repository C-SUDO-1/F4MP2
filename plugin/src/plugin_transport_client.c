#include "plugin_transport_client.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/select.h>
#endif

#include "message_ids.h"
#include "packet_codec.h"
#include "message_validation.h"
#include "protocol_constants.h"
#include "protocol_apply.h"

static size_t finalize_packet(PacketWriter* w, size_t payload_start) {
    uint16_t payload_len = (uint16_t)(pw_bytes_written(w) - payload_start);
    memcpy(w->buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return pw_bytes_written(w);
}

static bool ptc_send_one(PluginTransportClient* c, uint16_t type, uint16_t flags, bool (*enc)(PacketWriter*, const void*), const void* msg) {
    uint8_t tx[F4MP_MAX_PACKET_SIZE];
    PacketWriter w;
    PacketHeader ph = {0};
    MessageHeader mh = {0};
    size_t payload_start;
    if (!c || !enc) return false;
    ph.magic = F4MP_MAGIC;
    ph.version = F4MP_VERSION;
    ph.session_id = c->session_id;
    ph.sequence = ++c->sequence;
    ph.message_count = 1;
    mh.type = type;
    mh.flags = flags;
    pw_init(&w, tx, sizeof(tx));
    if (!encode_packet_header(&w, &ph)) return false;
    if (!encode_message_header(&w, &mh)) return false;
    payload_start = pw_bytes_written(&w);
    if (!enc(&w, msg)) return false;
    return st_send(&c->socket, tx, finalize_packet(&w, payload_start), &c->server_endpoint);
}

static bool enc_hello_wrap(PacketWriter* w, const void* m) {
    const MsgHello* h = (const MsgHello*)m;
    const char* fields = (const char*)(h + 1);
    const char* guid = fields;
    const char* name = guid + strlen(guid) + 1;
    return encode_msg_hello(w, h, guid, name);
}

static bool enc_player_state_wrap(PacketWriter* w, const void* m) {
    return encode_msg_player_state(w, (const MsgPlayerState*)m);
}

static bool enc_ping_wrap(PacketWriter* w, const void* m) {
    return encode_msg_ping(w, (const MsgPing*)m);
}

static bool enc_disconnect_wrap(PacketWriter* w, const void* m) {
    return encode_msg_disconnect(w, (const MsgDisconnect*)m);
}

static bool ptc_send_cached_hello(PluginTransportClient* c, bool is_retry) {
    struct {
        MsgHello hello;
        char strings[128];
    } payload;
    size_t g_len, n_len;
    char* p;
    if (!c || !c->hello_pending) return false;
    memset(&payload, 0, sizeof(payload));
    payload.hello.build_hash = c->hello_build_hash;
    payload.hello.capabilities = 0;
    g_len = strlen(c->hello_player_guid);
    n_len = strlen(c->hello_player_name);
    if (g_len + 1 + n_len + 1 > sizeof(payload.strings)) return false;
    p = payload.strings;
    memcpy(p, c->hello_player_guid, g_len + 1);
    p += g_len + 1;
    memcpy(p, c->hello_player_name, n_len + 1);
    if (!ptc_send_one(c, MSG_HELLO, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, enc_hello_wrap, &payload.hello)) return false;
    c->hello_send_count++;
    if (is_retry) c->hello_retry_count++;
    c->last_hello_poll_count = c->poll_count;
    return true;
}

static bool ptc_maybe_retry_hello(PluginTransportClient* c) {
    if (!c || c->connected || !c->hello_pending) return true;
    if (c->hello_send_count == 0) return ptc_send_cached_hello(c, false);
    if ((c->poll_count - c->last_hello_poll_count) < c->hello_retry_interval_polls) return true;
    return ptc_send_cached_hello(c, true);
}

bool ptc_open(PluginTransportClient* c, const char* host_ipv4, uint16_t port) {
    if (!c || !host_ipv4 || port == 0) return false;
    memset(c, 0, sizeof(*c));
    c->hello_retry_interval_polls = 4;
    if (!st_open_udp(&c->socket, 0)) return false;
    memset(&c->server_endpoint, 0, sizeof(c->server_endpoint));
    c->server_endpoint.addr.sin_family = AF_INET;
    c->server_endpoint.addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host_ipv4, &c->server_endpoint.addr.sin_addr) != 1) {
        st_close(&c->socket);
        return false;
    }
    c->server_endpoint.addr_len = sizeof(c->server_endpoint.addr);
    return true;
}

void ptc_close(PluginTransportClient* c) {
    if (!c) return;
    st_close(&c->socket);
    memset(c, 0, sizeof(*c));
}

bool ptc_send_hello(PluginTransportClient* c, uint32_t build_hash, const char* player_guid, const char* player_name) {
    if (!c || !player_guid || !player_name) return false;
    c->hello_build_hash = build_hash;
    strncpy(c->hello_player_guid, player_guid, sizeof(c->hello_player_guid) - 1);
    strncpy(c->hello_player_name, player_name, sizeof(c->hello_player_name) - 1);
    c->hello_pending = true;
    c->connected = false;
    c->local_player_id = 0;
    return ptc_send_cached_hello(c, false);
}

bool ptc_send_player_state(PluginTransportClient* c, const MsgPlayerState* msg) {
    return ptc_send_one(c, MSG_PLAYER_STATE, MSGF_CH1_WORLD, enc_player_state_wrap, msg);
}

bool ptc_send_ping(PluginTransportClient* c, uint32_t time_ms) {
    MsgPing msg;
    msg.time_ms = time_ms;
    if (!ptc_send_one(c, MSG_PING, MSGF_CH0_CONTROL, enc_ping_wrap, &msg)) return false;
    c->heartbeats_sent++;
    return true;
}

bool ptc_send_disconnect(PluginTransportClient* c, uint8_t reason) {
    MsgDisconnect msg;
    msg.reason = reason;
    return ptc_send_one(c, MSG_DISCONNECT, MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH0_CONTROL, enc_disconnect_wrap, &msg);
}

bool ptc_poll_once(PluginTransportClient* c, int timeout_ms, bool* out_applied_any) {
    fd_set rfds;
    struct timeval tv;
    uint8_t rx[F4MP_MAX_PACKET_SIZE];
    size_t rx_len;
    SocketEndpoint ep;
    PacketReader r;
    PacketHeader ph;
    MessageHeader mh;
    bool applied_any = false;
    if (!c) return false;
    c->poll_count++;
    FD_ZERO(&rfds);
    FD_SET(c->socket.fd, &rfds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
#ifdef _WIN32
    if (select(0, &rfds, NULL, NULL, &tv) <= 0) {
#else
    if (select((int)c->socket.fd + 1, &rfds, NULL, NULL, &tv) <= 0) {
#endif
        if (!ptc_maybe_retry_hello(c)) return false;
        if (out_applied_any) *out_applied_any = false;
        return true;
    }
    if (!st_recv(&c->socket, rx, sizeof(rx), &rx_len, &ep)) return false;
    pr_init(&r, rx, rx_len);
    if (!decode_packet_header(&r, &ph) || !f4mp_validate_packet_header(&ph)) return false;
    if (!decode_message_header(&r, &mh) || !f4mp_validate_message_header(&mh)) return false;
    c->session_id = ph.session_id;
    switch (mh.type) {
        case MSG_WELCOME: {
            MsgWelcome w;
            if (!decode_msg_welcome(&r, &w)) return false;
            c->connected = true;
            c->hello_pending = false;
            c->local_player_id = w.player_id;
            c->tick_rate = w.tick_rate;
            break;
        }
        case MSG_PONG: {
            MsgPong p;
            if (!decode_msg_pong(&r, &p)) return false;
            c->heartbeats_received++;
            c->last_pong_time_ms = p.time_ms;
            break;
        }
        default:
            if (!c->connected) c->pre_welcome_packet_count++;
            if (!plugin_apply_packet(rx, rx_len)) {
                if (!c->connected) {
                    c->ignored_pre_welcome_packet_count++;
                    if (!ptc_maybe_retry_hello(c)) return false;
                    if (out_applied_any) *out_applied_any = false;
                    return true;
                }
                return false;
            }
            applied_any = true;
            break;
    }
    if (!ptc_maybe_retry_hello(c)) return false;
    if (out_applied_any) *out_applied_any = applied_any;
    return true;
}

bool ptc_connected(const PluginTransportClient* c) { return c && c->connected; }
PlayerId ptc_local_player_id(const PluginTransportClient* c) { return c ? c->local_player_id : 0; }
uint32_t ptc_session_id(const PluginTransportClient* c) { return c ? c->session_id : 0; }

PluginTransportHandshakeStats ptc_get_handshake_stats(const PluginTransportClient* c) {
    PluginTransportHandshakeStats out;
    memset(&out, 0, sizeof(out));
    if (!c) return out;
    out.hello_pending = c->hello_pending;
    out.welcome_received = c->connected;
    out.poll_count = c->poll_count;
    out.hello_send_count = c->hello_send_count;
    out.hello_retry_count = c->hello_retry_count;
    out.pre_welcome_packet_count = c->pre_welcome_packet_count;
    out.ignored_pre_welcome_packet_count = c->ignored_pre_welcome_packet_count;
    return out;
}
