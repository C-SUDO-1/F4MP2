#include "movement_live_client_bridge.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_backend.h"
#include "fo4_scene_proxy_backend_stub.h"

#include <string.h>

static void mlcb_apply_defaults(MovementLiveClientBridge* b) {
    b->heartbeat_interval_ticks = 10;
    b->auto_step_remote_backend = true;
    b->owns_remote_backend = true;
    b->remote_backend_step_count = 2;
    b->apply_scene_backend_interp_config = false;
    b->scene_interp_config.enabled = true;
    b->scene_interp_config.position_alpha = 0.35f;
    b->scene_interp_config.rotation_alpha = 0.5f;
}

void mlcb_init(MovementLiveClientBridge* b) {
    if (!b) return;
    memset(b, 0, sizeof(*b));
    mlcb_apply_defaults(b);
}

void mlcb_set_config(MovementLiveClientBridge* b, const MovementLiveClientBridgeConfig* cfg) {
    if (!b || !cfg) return;
    b->heartbeat_interval_ticks = cfg->heartbeat_interval_ticks;
    b->auto_step_remote_backend = cfg->auto_step_remote_backend;
    b->owns_remote_backend = cfg->owns_remote_backend;
    b->remote_backend_step_count = (cfg->remote_backend_step_count == 0) ? 1u : cfg->remote_backend_step_count;
    b->apply_scene_backend_interp_config = cfg->apply_scene_backend_interp_config;
    b->scene_interp_config = cfg->scene_interp_config;
    if (b->apply_scene_backend_interp_config) {
        fo4_scene_proxy_backend_stub_set_interpolation_config(&b->scene_interp_config);
    }
}

MovementLiveClientBridgeConfig mlcb_get_config(const MovementLiveClientBridge* b) {
    MovementLiveClientBridgeConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    if (!b) return cfg;
    cfg.heartbeat_interval_ticks = b->heartbeat_interval_ticks;
    cfg.auto_step_remote_backend = b->auto_step_remote_backend;
    cfg.owns_remote_backend = b->owns_remote_backend;
    cfg.remote_backend_step_count = b->remote_backend_step_count;
    cfg.apply_scene_backend_interp_config = b->apply_scene_backend_interp_config;
    cfg.scene_interp_config = b->scene_interp_config;
    return cfg;
}

bool mlcb_open(MovementLiveClientBridge* b, const char* host_ipv4, uint16_t port, const char* player_guid, const char* player_name, uint32_t build_hash) {
    if (!b) return false;
    mlcb_init(b);
    if (b->apply_scene_backend_interp_config) {
        fo4_scene_proxy_backend_stub_set_interpolation_config(&b->scene_interp_config);
    }
    if (!ptc_open(&b->client, host_ipv4, port)) return false;
    if (!ptc_send_hello(&b->client, build_hash, player_guid, player_name)) {
        ptc_close(&b->client);
        return false;
    }
    return true;
}

void mlcb_close(MovementLiveClientBridge* b) {
    if (!b) return;
    if (b->client.connected) {
        (void)ptc_send_disconnect(&b->client, DISC_NONE);
        (void)ptc_poll_once(&b->client, 5, NULL);
    }
    ptc_close(&b->client);
    mlcb_init(b);
}

bool mlcb_tick(MovementLiveClientBridge* b, bool* out_sent, bool* out_applied) {
    MsgPlayerState msg;
    bool applied = false;
    bool sent = false;
    uint32_t i;
    if (!b) return false;
    if (!ptc_poll_once(&b->client, 5, &applied)) return false;
    if (ptc_connected(&b->client) && !b->joined) {
        lpo_init(ptc_local_player_id(&b->client));
        rpm_init(ptc_local_player_id(&b->client));
        b->joined = true;
    }
    b->tick_counter++;
    if (b->joined && b->heartbeat_interval_ticks > 0 && (b->tick_counter % b->heartbeat_interval_ticks) == 0) {
        if (!ptc_send_ping(&b->client, b->tick_counter)) return false;
    }
    if (b->joined && msb_build_player_state_if_changed(&msg)) {
        if (!ptc_send_player_state(&b->client, &msg)) return false;
        sent = true;
    }
    if (b->auto_step_remote_backend && b->owns_remote_backend) {
        for (i = 0; i < b->remote_backend_step_count; ++i) {
            rpb_step_backend();
        }
    }
    if (out_sent) *out_sent = sent;
    if (out_applied) *out_applied = applied;
    return true;
}

PlayerId mlcb_local_player_id(const MovementLiveClientBridge* b) {
    return b ? ptc_local_player_id(&b->client) : 0;
}

void mlcb_step_remote_backend(const MovementLiveClientBridge* b) {
    uint32_t i;
    if (!b || !b->auto_step_remote_backend || !b->owns_remote_backend) return;
    for (i = 0; i < b->remote_backend_step_count; ++i) {
        rpb_step_backend();
    }
}

void mlcb_apply_scene_backend_interpolation_config(const MovementLiveClientBridge* b) {
    if (!b || !b->apply_scene_backend_interp_config) return;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&b->scene_interp_config);
}
