#pragma once

#include <stdbool.h>

#include "plugin_live_transport_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct F4mpLiveTransportDevState {
    bool config_loaded;
    bool enabled;
    bool started;
    bool last_tick_sent;
    bool last_tick_applied;
    char config_path[F4MP_LIVE_CONFIG_PATH_CAP];
    F4mpLiveTransportConfig config;
    MovementLiveClientBridge bridge;
} F4mpLiveTransportDevState;

bool fltdev_init(void);
void fltdev_shutdown(void);
bool fltdev_tick(bool* out_sent, bool* out_applied);
bool fltdev_enabled(void);
bool fltdev_connected(void);
const char* fltdev_config_path(void);
F4mpLiveTransportDevState fltdev_get_state(void);

#ifdef __cplusplus
}
#endif
