#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "remote_proxy_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS 8

typedef enum Fo4SceneProxyLastCommand {
    FSP_CMD_NONE = 0,
    FSP_CMD_SPAWN = 1,
    FSP_CMD_UPDATE = 2,
    FSP_CMD_DESPAWN = 3
} Fo4SceneProxyLastCommand;

typedef struct Fo4SceneProxyPlayerState {
    bool in_use;
    PlayerId player_id;
    uint64_t actor_handle;
    bool visible;
    bool despawned;
    ProxyPlayerSpawnSpec last_spawn;
    ProxyPlayerUpdateSpec last_update;
    Vec3f presented_position;
    Rot3f presented_rotation;
    Vec3f target_position;
    Rot3f target_rotation;
    uint32_t transform_apply_count;
    uint32_t interpolation_tick_count;
    bool interpolation_pending;
    Fo4SceneProxyLastCommand last_command;
    uint32_t command_sequence;
    bool last_update_teleported;
    bool last_update_smoothed_position;
    bool last_update_smoothed_rotation;
    float last_update_source_distance;
} Fo4SceneProxyPlayerState;

void fo4_scene_proxy_backend_stub_install(void);
void fo4_scene_proxy_backend_stub_reset(void);
size_t fo4_scene_proxy_backend_stub_spawn_count(void);
size_t fo4_scene_proxy_backend_stub_update_count(void);
size_t fo4_scene_proxy_backend_stub_despawn_count(void);
uint32_t fo4_scene_proxy_backend_stub_total_command_sequence(void);
const Fo4SceneProxyPlayerState* fo4_scene_proxy_backend_stub_get(PlayerId player_id);

typedef struct Fo4SceneProxyInterpolationConfig {
    bool enabled;
    float position_alpha;
    float rotation_alpha;
} Fo4SceneProxyInterpolationConfig;

void fo4_scene_proxy_backend_stub_note_policy_result(PlayerId player_id, bool teleported, bool smoothed_position, bool smoothed_rotation, float source_distance);
void fo4_scene_proxy_backend_stub_set_interpolation_config(const Fo4SceneProxyInterpolationConfig* cfg);
Fo4SceneProxyInterpolationConfig fo4_scene_proxy_backend_stub_get_interpolation_config(void);
void fo4_scene_proxy_backend_stub_step(void);

#ifdef __cplusplus
}
#endif
