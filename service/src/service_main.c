#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#define f4mp_sleep_ms(ms) Sleep((DWORD)(ms))
#else
#include <unistd.h>
#define f4mp_sleep_ms(ms) usleep((ms) * 1000)
#endif

#include "session_manager.h"
#include "session_rules.h"
#include "world_state_store.h"
#include "transport_loop.h"
#include "replay_log.h"
#include "socket_transport.h"
#include "session_registry.h"
#include "workshop_whitelist.h"

typedef struct EndpointBinding {
    uint32_t connection_id;
    SocketEndpoint endpoint;
} EndpointBinding;

static EndpointBinding g_bindings[16];
static uint32_t g_next_connection_id = 100;

static void format_endpoint(const SocketEndpoint* ep, char* buffer, size_t buffer_size) {
    const char* addr_text = NULL;

    if (!buffer || buffer_size == 0) return;

    buffer[0] = '\0';
    if (!ep) {
        snprintf(buffer, buffer_size, "<null-endpoint>");
        return;
    }

    addr_text = inet_ntop(AF_INET, (const void*)&ep->addr.sin_addr, buffer, (socklen_t)buffer_size);
    if (!addr_text) {
        snprintf(buffer, buffer_size, "<invalid-ip>:%u", (unsigned)ntohs(ep->addr.sin_port));
        return;
    }

    {
        size_t used = strlen(buffer);
        if (used + 1 < buffer_size) {
            (void)snprintf(buffer + used, buffer_size - used, ":%u", (unsigned)ntohs(ep->addr.sin_port));
        }
    }
}

static void log_udp_payload_preview(const char* label, const SocketEndpoint* ep, const uint8_t* bytes, size_t len) {
    char endpoint[64];
    size_t preview_len = 0;
    size_t i;

    format_endpoint(ep, endpoint, sizeof(endpoint));
    preview_len = len < 16 ? len : 16;

    fprintf(stderr, "%s: %zu byte(s) from %s", label, len, endpoint);
    if (preview_len > 0 && bytes) {
        fprintf(stderr, " [");
        for (i = 0; i < preview_len; ++i) {
            fprintf(stderr, "%s%02X", (i == 0 ? "" : " "), (unsigned)bytes[i]);
        }
        if (len > preview_len) fprintf(stderr, " ...");
        fprintf(stderr, "]");
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

static uint32_t get_connection_id_for_endpoint(const SocketEndpoint* ep) {
    size_t i;
    for (i = 0; i < sizeof(g_bindings)/sizeof(g_bindings[0]); ++i) {
        if (g_bindings[i].connection_id && st_endpoint_equal(&g_bindings[i].endpoint, ep)) {
            return g_bindings[i].connection_id;
        }
    }
    for (i = 0; i < sizeof(g_bindings)/sizeof(g_bindings[0]); ++i) {
        if (!g_bindings[i].connection_id) {
            g_bindings[i].connection_id = g_next_connection_id++;
            g_bindings[i].endpoint = *ep;
            return g_bindings[i].connection_id;
        }
    }
    return 0;
}

static const SocketEndpoint* get_endpoint_for_connection(uint32_t connection_id) {
    size_t i;
    for (i = 0; i < sizeof(g_bindings)/sizeof(g_bindings[0]); ++i) {
        if (g_bindings[i].connection_id == connection_id) return &g_bindings[i].endpoint;
    }
    return NULL;
}

int main(int argc, char** argv) {
    SessionState* session;
    TransportLoopContext loop;
    SocketTransport transport;
    uint8_t rx[1400];
    size_t rx_len;
    SocketEndpoint ep;
    uint16_t port = 0;

    const char* whitelist_path = NULL;
    const char* timeout_ticks_env = NULL;

    sm_init();
    sr_init();
    replay_log_open("service.log");
    workshop_whitelist_init_default();

    session = sm_create_session(1, RULE_BUBBLE_ONLY | RULE_HOST_AUTH_COMBAT | RULE_HOST_AUTH_WORKSHOP | RULE_GUEST_PROFILE_ENABLED, 20, 0);
    if (!session) {
        fprintf(stderr, "failed to create session\n");
        return 1;
    }

    ws_alloc_settlement(session, 1001, (Vec3f){0.0f, 0.0f, 0.0f}, 2048.0f, 512);
    sm_spawn_default_hostile(session, 0x90000001u, (Vec3f){128.0f, 0.0f, 0.0f});
    tl_init(&loop, session);
    timeout_ticks_env = getenv("F4MP_PLAYER_TIMEOUT_TICKS");
    if (timeout_ticks_env && timeout_ticks_env[0] != '\0') {
        tl_set_player_timeout_ticks(&loop, (uint32_t)atoi(timeout_ticks_env));
    }

    whitelist_path = getenv("F4MP_WORKSHOP_WHITELIST");
    if (argc > 2) whitelist_path = argv[2];
    if (whitelist_path) {
        if (workshop_whitelist_load_json_file(whitelist_path)) {
            printf("loaded workshop whitelist from %s (%zu entries)\n", whitelist_path, workshop_whitelist_count());
        } else {
            printf("failed to load workshop whitelist from %s, keeping defaults (%zu entries)\n", whitelist_path, workshop_whitelist_count());
        }
    }

    if (argc > 1) port = (uint16_t)atoi(argv[1]);
    if (port == 0) {
        printf("fo4-coop service scaffold started. pass UDP port to run live transport.\n");
        replay_log_close();
        return 0;
    }

    if (!st_open_udp(&transport, port)) {
        fprintf(stderr, "failed to bind UDP %u\n", (unsigned)port);
        replay_log_close();
        return 1;
    }

    printf("fo4-coop service listening on UDP %u\n", (unsigned)port);
    while (1) {
        TransportEnvelope env;
        bool handled = false;
        size_t i;
        if (!st_recv(&transport, rx, sizeof(rx), &rx_len, &ep)) continue;
        log_udp_payload_preview("udp rx", &ep, rx, rx_len);
        memset(&env, 0, sizeof(env));
        env.connection_id = get_connection_id_for_endpoint(&ep);
        if (env.connection_id == 0) {
            fprintf(stderr, "udp drop: endpoint binding table full\n");
            fflush(stderr);
            continue;
        }
        env.length = rx_len;
        memcpy(env.bytes, rx, rx_len);
        tl_clear_outputs(&loop);
        handled = tl_handle_envelope(&loop, &env);
        if (!handled) {
            log_udp_payload_preview("udp drop", &ep, rx, rx_len);
            continue;
        }
        tl_tick(&loop);
        for (i = 0; i < tl_get_output_count(&loop); ++i) {
            const TransportOutput* out = tl_get_output(&loop, i);
            const SocketEndpoint* dst = get_endpoint_for_connection(out->connection_id);
            if (dst) {
                log_udp_payload_preview("udp tx", dst, out->bytes, out->length);
                (void)st_send(&transport, out->bytes, out->length, dst);
            }
        }
        f4mp_sleep_ms(10);
    }

    st_close(&transport);
    replay_log_close();
    return 0;
}
