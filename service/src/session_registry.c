#include "session_registry.h"
#include <string.h>

#define F4MP_MAX_REGISTRY 32
static SessionRegistryEntry g_registry[F4MP_MAX_REGISTRY];

void sr_init(void) {
    memset(g_registry, 0, sizeof(g_registry));
}

bool sr_find_player_id(const char* player_guid, PlayerId* out_player_id) {
    size_t i;
    if (!player_guid || !out_player_id) return false;
    for (i = 0; i < F4MP_MAX_REGISTRY; ++i) {
        if (g_registry[i].in_use && strcmp(g_registry[i].player_guid, player_guid) == 0) {
            *out_player_id = g_registry[i].player_id;
            return true;
        }
    }
    return false;
}

void sr_bind_player(const char* player_guid, PlayerId player_id) {
    size_t i;
    if (!player_guid || !player_guid[0]) return;
    for (i = 0; i < F4MP_MAX_REGISTRY; ++i) {
        if (g_registry[i].in_use && strcmp(g_registry[i].player_guid, player_guid) == 0) {
            g_registry[i].player_id = player_id;
            return;
        }
    }
    for (i = 0; i < F4MP_MAX_REGISTRY; ++i) {
        if (!g_registry[i].in_use) {
            g_registry[i].in_use = true;
            strncpy(g_registry[i].player_guid, player_guid, sizeof(g_registry[i].player_guid) - 1);
            g_registry[i].player_id = player_id;
            return;
        }
    }
}

void sr_unbind_player(const char* player_guid) {
    size_t i;
    if (!player_guid) return;
    for (i = 0; i < F4MP_MAX_REGISTRY; ++i) {
        if (g_registry[i].in_use && strcmp(g_registry[i].player_guid, player_guid) == 0) {
            memset(&g_registry[i], 0, sizeof(g_registry[i]));
            return;
        }
    }
}
