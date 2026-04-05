#include <assert.h>
#include <stdbool.h>
#include "remote_proxy_backend.h"

static int g_steps = 0;
static bool spawn_ok(void* u, const ProxyPlayerSpawnSpec* s){ (void)u; (void)s; return true; }
static bool update_ok(void* u, const ProxyPlayerUpdateSpec* s){ (void)u; (void)s; return true; }
static bool despawn_ok(void* u, PlayerId p){ (void)u; (void)p; return true; }
static void step_tick(void* u){ (void)u; g_steps++; }

int main(void) {
    RemoteProxyBackend b;
    b.user = 0;
    b.spawn_player = spawn_ok;
    b.update_player = update_ok;
    b.despawn_player = despawn_ok;
    b.step = step_tick;
    rpb_install_backend(&b);
    rpb_step_backend();
    rpb_step_backend();
    assert(g_steps == 2);
    return 0;
}
