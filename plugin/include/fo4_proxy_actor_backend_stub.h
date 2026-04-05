#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "remote_proxy_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FO4_PROXY_BACKEND_STUB_MAX_PLAYERS 8

typedef struct Fo4ProxyBackendPlayerState {
    bool in_use;
    PlayerId player_id;
    ProxyPlayerSpawnSpec spawn;
    ProxyPlayerUpdateSpec update;
    bool despawned;
} Fo4ProxyBackendPlayerState;

void fo4_proxy_backend_stub_install(void);
void fo4_proxy_backend_stub_reset(void);
size_t fo4_proxy_backend_stub_spawn_count(void);
size_t fo4_proxy_backend_stub_update_count(void);
size_t fo4_proxy_backend_stub_despawn_count(void);
const Fo4ProxyBackendPlayerState* fo4_proxy_backend_stub_get(PlayerId player_id);

#ifdef __cplusplus
}
#endif
