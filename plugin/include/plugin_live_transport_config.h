#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "movement_live_client_bridge.h"
#include "protocol_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F4MP_LIVE_CONFIG_PATH_CAP 260u

typedef struct F4mpLiveTransportConfig {
    bool dev_enabled;
    char host_ipv4[64];
    uint16_t port;
    char player_guid[F4MP_MAX_PLAYER_GUID_BYTES + 1];
    char player_name[F4MP_MAX_PLAYER_NAME_BYTES + 1];
    uint32_t build_hash;
    MovementLiveClientBridgeConfig bridge;
} F4mpLiveTransportConfig;

void fltcfg_set_defaults(F4mpLiveTransportConfig* cfg);
bool fltcfg_load_file(const char* path, F4mpLiveTransportConfig* out_cfg);
bool fltcfg_load_default(F4mpLiveTransportConfig* out_cfg, char* out_path, size_t out_path_cap);

#ifdef __cplusplus
}
#endif
