#include "socket_transport.h"

#include <string.h>

#ifdef _WIN32
static bool st_platform_init(void) {
    static bool initialized = false;
    if (initialized) return true;
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) return false;
    initialized = true;
    return true;
}
#else
static bool st_platform_init(void) {
    return true;
}
#endif

bool st_open_udp(SocketTransport* t, uint16_t port) {
    struct sockaddr_in addr;
    int on = 1;
    if (!t) return false;
    memset(t, 0, sizeof(*t));
    t->fd = F4MP_INVALID_SOCKET;
    if (!st_platform_init()) return false;
    t->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (t->fd == F4MP_INVALID_SOCKET) return false;
#ifdef _WIN32
    setsockopt(t->fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
#else
    setsockopt(t->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(t->fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        f4mp_socket_close(t->fd);
        t->fd = F4MP_INVALID_SOCKET;
        return false;
    }
    t->bound_port = port;
    return true;
}

void st_close(SocketTransport* t) {
    if (t && t->fd != F4MP_INVALID_SOCKET) {
        f4mp_socket_close(t->fd);
        t->fd = F4MP_INVALID_SOCKET;
    }
}

bool st_recv(SocketTransport* t, uint8_t* buffer, size_t capacity, size_t* out_len, SocketEndpoint* out_ep) {
#ifdef _WIN32
    int n;
#else
    ssize_t n;
#endif
    if (!t || t->fd == F4MP_INVALID_SOCKET || !buffer || !out_len || !out_ep) return false;
    out_ep->addr_len = sizeof(out_ep->addr);
    n = recvfrom(t->fd, buffer, capacity, 0, (struct sockaddr*)&out_ep->addr, &out_ep->addr_len);
    if (n <= 0) return false;
    *out_len = (size_t)n;
    return true;
}

bool st_send(SocketTransport* t, const uint8_t* buffer, size_t len, const SocketEndpoint* ep) {
#ifdef _WIN32
    int n;
#else
    ssize_t n;
#endif
    if (!t || t->fd == F4MP_INVALID_SOCKET || !buffer || !ep) return false;
    n = sendto(t->fd, buffer, len, 0, (const struct sockaddr*)&ep->addr, ep->addr_len);
#ifdef _WIN32
    return n == (int)len;
#else
    return n == (ssize_t)len;
#endif
}

bool st_endpoint_equal(const SocketEndpoint* a, const SocketEndpoint* b) {
    return a && b &&
           a->addr.sin_family == b->addr.sin_family &&
           a->addr.sin_port == b->addr.sin_port &&
           a->addr.sin_addr.s_addr == b->addr.sin_addr.s_addr;
}
