#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "plugin_transport_client.h"
#include "movement_sync_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MovementLiveClientBridge {
    PluginTransportClient client;
    bool joined;
    uint32_t tick_counter;
    uint32_t heartbeat_interval_ticks;
    bool auto_step_remote_backend;
    bool owns_remote_backend;
    uint32_t remote_backend_step_count;
    bool apply_scene_backend_interp_config;
    Fo4SceneProxyInterpolationConfig scene_interp_config;
} MovementLiveClientBridge;

typedef struct MovementLiveClientBridgeConfig {
    uint32_t heartbeat_interval_ticks;
    bool auto_step_remote_backend;
    bool owns_remote_backend;
    uint32_t remote_backend_step_count;
    bool apply_scene_backend_interp_config;
    Fo4SceneProxyInterpolationConfig scene_interp_config;
} MovementLiveClientBridgeConfig;

void mlcb_init(MovementLiveClientBridge* b);
void mlcb_set_config(MovementLiveClientBridge* b, const MovementLiveClientBridgeConfig* cfg);
MovementLiveClientBridgeConfig mlcb_get_config(const MovementLiveClientBridge* b);

bool mlcb_open(MovementLiveClientBridge* b, const char* host_ipv4, uint16_t port, const char* player_guid, const char* player_name, uint32_t build_hash);
void mlcb_close(MovementLiveClientBridge* b);
bool mlcb_tick(MovementLiveClientBridge* b, bool* out_sent, bool* out_applied);
PlayerId mlcb_local_player_id(const MovementLiveClientBridge* b);
void mlcb_step_remote_backend(const MovementLiveClientBridge* b);
void mlcb_apply_scene_backend_interpolation_config(const MovementLiveClientBridge* b);

#ifdef __cplusplus
}
#endif
