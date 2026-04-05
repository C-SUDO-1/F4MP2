#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "fo4_proxy_actor_driver.h"
#include "plugin_proxy_actor_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fo4ProxyActorSpawnContractConfig {
    PlayerId local_player_id;
    uint32_t default_proxy_base_form_id;
    bool suppress_self_spawn;
    bool require_base_form_id;
} Fo4ProxyActorSpawnContractConfig;

typedef struct Fo4ProxyActorSpawnContractState {
    Fo4ProxyActorSpawnContractConfig config;
    bool configured;
    uint32_t plan_count;
    uint32_t spawn_count;
    uint32_t reject_count;
    PlayerId last_player_id;
    bool last_used_default_base_form;
    bool last_suppressed_self_spawn;
    Fo4ProxyActorCreateSpec last_create_spec;
} Fo4ProxyActorSpawnContractState;

void fpasc_reset(void);
void fpasc_configure(const Fo4ProxyActorSpawnContractConfig* config);
bool fpasc_build_create_spec(const ProxyPlayerSpawnSpec* spec, Fo4ProxyActorCreateSpec* out_create_spec);
bool fpasc_spawn_via_driver(const ProxyPlayerSpawnSpec* spec, uint64_t actor_handle);
Fo4ProxyActorSpawnContractState fpasc_state(void);

#ifdef __cplusplus
}
#endif
