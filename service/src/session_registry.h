#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "service_state_models.h"

typedef struct SessionRegistryEntry {
    bool     in_use;
    char     player_guid[64];
    PlayerId player_id;
} SessionRegistryEntry;

void sr_init(void);
bool sr_find_player_id(const char* player_guid, PlayerId* out_player_id);
void sr_bind_player(const char* player_guid, PlayerId player_id);
void sr_unbind_player(const char* player_guid);
