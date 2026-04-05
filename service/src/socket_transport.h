#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET f4mp_socket_t;
typedef int socklen_t;
#define F4MP_INVALID_SOCKET INVALID_SOCKET
#define F4MP_SOCKET_ERROR_VALUE SOCKET_ERROR
#define f4mp_socket_close closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int f4mp_socket_t;
#define F4MP_INVALID_SOCKET (-1)
#define F4MP_SOCKET_ERROR_VALUE (-1)
#define f4mp_socket_close close
#endif

typedef struct SocketEndpoint {
    struct sockaddr_in addr;
    socklen_t addr_len;
} SocketEndpoint;

typedef struct SocketTransport {
    f4mp_socket_t fd;
    uint16_t bound_port;
} SocketTransport;

bool st_open_udp(SocketTransport* t, uint16_t port);
void st_close(SocketTransport* t);
bool st_recv(SocketTransport* t, uint8_t* buffer, size_t capacity, size_t* out_len, SocketEndpoint* out_ep);
bool st_send(SocketTransport* t, const uint8_t* buffer, size_t len, const SocketEndpoint* ep);
bool st_endpoint_equal(const SocketEndpoint* a, const SocketEndpoint* b);
