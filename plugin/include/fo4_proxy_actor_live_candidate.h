#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "fo4_proxy_actor_spawn_contract.h"
#include "messages_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fo4ProxyActorLiveCandidateConfig {
    PlayerId local_player_id;
    uint32_t default_proxy_base_form_id;
    bool suppress_self_spawn;
    bool require_base_form_id;
    const char* actor_class_name;
    const char* create_api_symbol;
    const char* move_api_symbol;
    const char* rotate_api_symbol;
    const char* despawn_api_symbol;
} Fo4ProxyActorLiveCandidateConfig;

typedef struct Fo4ProxyActorLiveCandidateState {
    bool configured;
    bool enabled;
    const char* actor_class_name;
    const char* create_api_symbol;
    const char* move_api_symbol;
    const char* rotate_api_symbol;
    const char* despawn_api_symbol;
    uint32_t spawn_count;
    uint32_t move_count;
    uint32_t rotate_count;
    uint32_t despawn_count;
    PlayerId last_player_id;
    uint64_t last_actor_handle;
    uint32_t last_proxy_base_form_id;
    bool has_driver_state;
} Fo4ProxyActorLiveCandidateState;

void fpalc_reset(void);
void fpalc_configure(const Fo4ProxyActorLiveCandidateConfig* config);
bool fpalc_enabled(void);
bool fpalc_spawn_via_driver(const ProxyPlayerSpawnSpec* spec, uint64_t actor_handle);
bool fpalc_apply_presented_state(const MsgPlayerState* msg, uint64_t actor_handle);
bool fpalc_despawn(PlayerId player_id);
Fo4ProxyActorLiveCandidateState fpalc_state(void);

#ifdef __cplusplus
}
#endif
