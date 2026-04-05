#pragma once

#include <stdbool.h>
#include "plugin_proxy_actor_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RemoteProxyBackend {
    void* user;
    bool (*spawn_player)(void* user, const ProxyPlayerSpawnSpec* spec);
    bool (*update_player)(void* user, const ProxyPlayerUpdateSpec* spec);
    bool (*despawn_player)(void* user, PlayerId player_id);
    void (*step)(void* user);
} RemoteProxyBackend;

void rpb_install_backend(const RemoteProxyBackend* backend);
void rpb_reset_default_backend(void);
const RemoteProxyBackend* rpb_get_backend(void);
void rpb_step_backend(void);

#ifdef __cplusplus
}
#endif
