#include "workshop_whitelist.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static WorkshopWhitelistEntry g_entries[F4MP_MAX_WORKSHOP_WHITELIST];
static size_t g_entry_count = 0;

static void add_default(uint32_t form_id, const char* editor_id, bool movable, bool scrappable,
                        bool snap_enabled, bool animated, bool scripted) {
    WorkshopWhitelistEntry* e;
    if (g_entry_count >= F4MP_MAX_WORKSHOP_WHITELIST) return;
    e = &g_entries[g_entry_count++];
    memset(e, 0, sizeof(*e));
    e->form_id = form_id;
    snprintf(e->editor_id, sizeof(e->editor_id), "%s", editor_id ? editor_id : "");
    e->movable = movable;
    e->scrappable = scrappable;
    e->snap_enabled = snap_enabled;
    e->animated = animated;
    e->scripted = scripted;
}

void workshop_whitelist_init_default(void) {
    g_entry_count = 0;
    add_default(0xAAAAAAAAu, "WoodWall01",       true, true, true,  false, false);
    add_default(0xAAAAAAABu, "WoodWallWindow01", true, true, true,  false, false);
    add_default(0xBBBBBBBBu, "WoodFloor01",      true, true, true,  false, false);
    add_default(0xCCCCCCCCu, "WoodRoofFlat01",   true, true, true,  false, false);
    add_default(0xDDDDDDDDu, "WoodStairs01",     true, true, true,  false, false);
    add_default(0xEEEEEEEEu, "FenceWire01",      true, true, true,  false, false);
    add_default(0xFFFFFFFFu, "SandbagWall01",    true, true, false, false, false);
}

static bool parse_bool_value(const char* s) {
    return s && strncmp(s, "true", 4) == 0;
}

static const char* skip_ws(const char* s) {
    while (*s && isspace((unsigned char)*s)) ++s;
    return s;
}

static const char* find_json_key(const char* start, const char* key) {
    char needle[96];
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    return strstr(start, needle);
}

static bool parse_string_field(const char* obj, const char* key, char* out, size_t out_cap) {
    const char* p = find_json_key(obj, key);
    const char* q;
    size_t len;
    if (!p) return false;
    p = strchr(p, ':');
    if (!p) return false;
    p = skip_ws(p + 1);
    if (*p != '"') return false;
    ++p;
    q = strchr(p, '"');
    if (!q) return false;
    len = (size_t)(q - p);
    if (len >= out_cap) len = out_cap - 1;
    memcpy(out, p, len);
    out[len] = '\0';
    return true;
}

static bool parse_hex_field(const char* obj, const char* key, uint32_t* out) {
    char buf[64];
    char* endptr = NULL;
    unsigned long value;
    if (!parse_string_field(obj, key, buf, sizeof(buf))) return false;
    value = strtoul(buf, &endptr, 0);
    if (endptr == buf) return false;
    *out = (uint32_t)value;
    return true;
}

static bool parse_bool_field(const char* obj, const char* key, bool* out) {
    const char* p = find_json_key(obj, key);
    if (!p) return false;
    p = strchr(p, ':');
    if (!p) return false;
    p = skip_ws(p + 1);
    *out = parse_bool_value(p);
    return true;
}

bool workshop_whitelist_load_json_file(const char* path) {
    FILE* f;
    long size;
    char* buf;
    char* p;
    size_t loaded = 0;

    if (!path) return false;
    f = fopen(path, "rb");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    size = ftell(f);
    if (size <= 0) { fclose(f); return false; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return false; }
    buf = (char*)malloc((size_t)size + 1);
    if (!buf) { fclose(f); return false; }
    if (fread(buf, 1, (size_t)size, f) != (size_t)size) { free(buf); fclose(f); return false; }
    fclose(f);
    buf[size] = '\0';

    g_entry_count = 0;
    p = buf;
    while ((p = strstr(p, "\"editor_id\"")) != NULL) {
        WorkshopWhitelistEntry entry;
        memset(&entry, 0, sizeof(entry));
        if (parse_string_field(p, "editor_id", entry.editor_id, sizeof(entry.editor_id)) &&
            parse_hex_field(p, "form_id_hex", &entry.form_id) &&
            parse_bool_field(p, "movable", &entry.movable) &&
            parse_bool_field(p, "scrappable", &entry.scrappable) &&
            parse_bool_field(p, "snap_enabled", &entry.snap_enabled) &&
            parse_bool_field(p, "animated", &entry.animated) &&
            parse_bool_field(p, "scripted", &entry.scripted)) {
            if (g_entry_count < F4MP_MAX_WORKSHOP_WHITELIST) {
                g_entries[g_entry_count++] = entry;
                ++loaded;
            }
        }
        p += 10;
    }

    free(buf);
    if (loaded == 0) {
        workshop_whitelist_init_default();
        return false;
    }
    return true;
}

size_t workshop_whitelist_count(void) {
    return g_entry_count;
}

bool workshop_whitelist_contains(uint32_t form_id) {
    size_t i;
    for (i = 0; i < g_entry_count; ++i) if (g_entries[i].form_id == form_id) return true;
    return false;
}

const WorkshopWhitelistEntry* workshop_whitelist_lookup(uint32_t form_id) {
    size_t i;
    for (i = 0; i < g_entry_count; ++i) if (g_entries[i].form_id == form_id) return &g_entries[i];
    return NULL;
}

bool workshop_whitelist_has_placeholder_ids(void) {
    size_t i;
    for (i = 0; i < g_entry_count; ++i) {
        uint32_t id = g_entries[i].form_id;
        if (id == 0xAAAAAAAAu || id == 0xAAAAAAABu || id == 0xBBBBBBBBu ||
            id == 0xCCCCCCCCu || id == 0xDDDDDDDDu || id == 0xEEEEEEEEu ||
            id == 0xFFFFFFFFu) {
            return true;
        }
    }
    return false;
}
