#include "plugin_live_transport_dev.h"

#include <string.h>

static F4mpLiveTransportDevState g_state;

static void reset_state(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool fltdev_init(void) {
    MovementLiveClientBridgeConfig bridge_cfg;
    if (g_state.started || g_state.config_loaded) return true;

    reset_state();
    (void)fltcfg_load_default(&g_state.config, g_state.config_path, sizeof(g_state.config_path));
    g_state.config_loaded = true;
    g_state.enabled = g_state.config.dev_enabled;
    mlcb_init(&g_state.bridge);

    if (!g_state.enabled) return true;
    if (!mlcb_open(&g_state.bridge,
                   g_state.config.host_ipv4,
                   g_state.config.port,
                   g_state.config.player_guid,
                   g_state.config.player_name,
                   g_state.config.build_hash)) {
        reset_state();
        return false;
    }

    bridge_cfg = g_state.config.bridge;
    mlcb_set_config(&g_state.bridge, &bridge_cfg);
    g_state.started = true;
    return true;
}

void fltdev_shutdown(void) {
    if (g_state.started) {
        mlcb_close(&g_state.bridge);
    }
    reset_state();
}

bool fltdev_tick(bool* out_sent, bool* out_applied) {
    bool sent = false;
    bool applied = false;
    if (!g_state.config_loaded) {
        if (!fltdev_init()) return false;
    }
    if (!g_state.enabled || !g_state.started) {
        if (out_sent) *out_sent = false;
        if (out_applied) *out_applied = false;
        return true;
    }
    if (!mlcb_tick(&g_state.bridge, &sent, &applied)) return false;
    g_state.last_tick_sent = sent;
    g_state.last_tick_applied = applied;
    if (out_sent) *out_sent = sent;
    if (out_applied) *out_applied = applied;
    return true;
}

bool fltdev_enabled(void) {
    if (!g_state.config_loaded) {
        if (!fltdev_init()) return false;
    }
    return g_state.enabled;
}

bool fltdev_connected(void) {
    if (!g_state.config_loaded) {
        if (!fltdev_init()) return false;
    }
    return g_state.started && ptc_connected(&g_state.bridge.client);
}

const char* fltdev_config_path(void) {
    if (!g_state.config_loaded) {
        if (!fltdev_init()) return "";
    }
    return g_state.config_path;
}

F4mpLiveTransportDevState fltdev_get_state(void) {
    if (!g_state.config_loaded) {
        (void)fltdev_init();
    }
    return g_state;
}
