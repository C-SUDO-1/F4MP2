#include "fo4_proxy_actor_backend_stub.h"
#include <string.h>

static struct {
    Fo4ProxyBackendPlayerState players[FO4_PROXY_BACKEND_STUB_MAX_PLAYERS];
    size_t spawn_count;
    size_t update_count;
    size_t despawn_count;
} g_stub;

static Fo4ProxyBackendPlayerState* find_slot(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_PROXY_BACKEND_STUB_MAX_PLAYERS; ++i) {
        if (g_stub.players[i].in_use && g_stub.players[i].player_id == player_id) return &g_stub.players[i];
    }
    for (i = 0; i < FO4_PROXY_BACKEND_STUB_MAX_PLAYERS; ++i) {
        if (!g_stub.players[i].in_use) return &g_stub.players[i];
    }
    return 0;
}

static bool stub_spawn(void* user, const ProxyPlayerSpawnSpec* spec) {
    Fo4ProxyBackendPlayerState* s;
    (void)user;
    if (!spec) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->player_id = spec->player_id;
    s->spawn = *spec;
    g_stub.spawn_count++;
    return true;
}

static bool stub_update(void* user, const ProxyPlayerUpdateSpec* spec) {
    Fo4ProxyBackendPlayerState* s;
    (void)user;
    if (!spec) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    if (!s->in_use) {
        memset(s, 0, sizeof(*s));
        s->in_use = true;
        s->player_id = spec->player_id;
    }
    s->update = *spec;
    g_stub.update_count++;
    return true;
}

static bool stub_despawn(void* user, PlayerId player_id) {
    Fo4ProxyBackendPlayerState* s;
    (void)user;
    s = find_slot(player_id);
    if (!s || !s->in_use || s->player_id != player_id) return false;
    s->despawned = true;
    g_stub.despawn_count++;
    return true;
}

void fo4_proxy_backend_stub_reset(void) {
    memset(&g_stub, 0, sizeof(g_stub));
}

void fo4_proxy_backend_stub_install(void) {
    RemoteProxyBackend b;
    fo4_proxy_backend_stub_reset();
    memset(&b, 0, sizeof(b));
    b.user = 0;
    b.spawn_player = stub_spawn;
    b.update_player = stub_update;
    b.despawn_player = stub_despawn;
    b.step = 0;
    rpb_install_backend(&b);
}

size_t fo4_proxy_backend_stub_spawn_count(void) { return g_stub.spawn_count; }
size_t fo4_proxy_backend_stub_update_count(void) { return g_stub.update_count; }
size_t fo4_proxy_backend_stub_despawn_count(void) { return g_stub.despawn_count; }
const Fo4ProxyBackendPlayerState* fo4_proxy_backend_stub_get(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_PROXY_BACKEND_STUB_MAX_PLAYERS; ++i) {
        if (g_stub.players[i].in_use && g_stub.players[i].player_id == player_id) return &g_stub.players[i];
    }
    return 0;
}
