#include "remote_proxy_backend.h"
#include "proxy_runtime.h"
#include <string.h>

static bool default_spawn(void* user, const ProxyPlayerSpawnSpec* spec) {
    (void)user;
    return proxy_spawn_player(spec);
}

static bool default_update(void* user, const ProxyPlayerUpdateSpec* spec) {
    (void)user;
    return proxy_update_player(spec);
}

static bool default_despawn(void* user, PlayerId player_id) {
    (void)user;
    return proxy_despawn_player(player_id);
}

static void default_step(void* user) {
    (void)user;
}

static RemoteProxyBackend g_backend;

void rpb_reset_default_backend(void) {
    memset(&g_backend, 0, sizeof(g_backend));
    g_backend.spawn_player = default_spawn;
    g_backend.update_player = default_update;
    g_backend.despawn_player = default_despawn;
    g_backend.step = default_step;
}

void rpb_install_backend(const RemoteProxyBackend* backend) {
    if (!backend) {
        rpb_reset_default_backend();
        return;
    }
    g_backend = *backend;
}

const RemoteProxyBackend* rpb_get_backend(void) {
    if (!g_backend.spawn_player || !g_backend.update_player || !g_backend.despawn_player) {
        rpb_reset_default_backend();
    }
    return &g_backend;
}

void rpb_step_backend(void) {
    const RemoteProxyBackend* b = rpb_get_backend();
    if (b && b->step) b->step(b->user);
}
