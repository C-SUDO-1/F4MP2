#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define F4MP_MAX_WORKSHOP_WHITELIST 256

typedef struct WorkshopWhitelistEntry {
    uint32_t form_id;
    char editor_id[64];
    bool movable;
    bool scrappable;
    bool snap_enabled;
    bool animated;
    bool scripted;
} WorkshopWhitelistEntry;

void workshop_whitelist_init_default(void);
bool workshop_whitelist_load_json_file(const char* path);
size_t workshop_whitelist_count(void);
bool workshop_whitelist_contains(uint32_t form_id);
const WorkshopWhitelistEntry* workshop_whitelist_lookup(uint32_t form_id);
bool workshop_whitelist_has_placeholder_ids(void);

