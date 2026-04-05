#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "socket_transport.h"
#include "messages_control.h"
#include "messages_player.h"
#include "protocol_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PluginTransportHandshakeStats {
    bool hello_pending;
    bool welcome_received;
    uint32_t poll_count;
    uint32_t hello_send_count;
    uint32_t hello_retry_count;
    uint32_t pre_welcome_packet_count;
    uint32_t ignored_pre_welcome_packet_count;
} PluginTransportHandshakeStats;

typedef struct PluginTransportClient {
    SocketTransport socket;
    SocketEndpoint server_endpoint;
    bool connected;
    uint32_t session_id;
    PlayerId local_player_id;
    uint32_t sequence;
    uint16_t tick_rate;
    uint32_t heartbeats_sent;
    uint32_t heartbeats_received;
    uint32_t last_pong_time_ms;

    bool hello_pending;
    uint32_t hello_build_hash;
    char hello_player_guid[F4MP_MAX_PLAYER_GUID_BYTES + 1];
    char hello_player_name[F4MP_MAX_PLAYER_NAME_BYTES + 1];
    uint32_t hello_send_count;
    uint32_t hello_retry_count;
    uint32_t pre_welcome_packet_count;
    uint32_t ignored_pre_welcome_packet_count;
    uint32_t poll_count;
    uint32_t last_hello_poll_count;
    uint32_t hello_retry_interval_polls;
} PluginTransportClient;

bool ptc_open(PluginTransportClient* c, const char* host_ipv4, uint16_t port);
void ptc_close(PluginTransportClient* c);

bool ptc_send_hello(PluginTransportClient* c, uint32_t build_hash, const char* player_guid, const char* player_name);
bool ptc_send_player_state(PluginTransportClient* c, const MsgPlayerState* msg);
bool ptc_send_ping(PluginTransportClient* c, uint32_t time_ms);
bool ptc_send_disconnect(PluginTransportClient* c, uint8_t reason);
bool ptc_poll_once(PluginTransportClient* c, int timeout_ms, bool* out_applied_any);

bool ptc_connected(const PluginTransportClient* c);
PlayerId ptc_local_player_id(const PluginTransportClient* c);
uint32_t ptc_session_id(const PluginTransportClient* c);
PluginTransportHandshakeStats ptc_get_handshake_stats(const PluginTransportClient* c);

#ifdef __cplusplus
}
#endif
