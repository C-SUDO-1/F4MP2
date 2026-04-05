#include "plugin_live_transport_config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

static bool file_exists(const char* path) {
    FILE* f;
    if (!path || !path[0]) return false;
    f = fopen(path, "rb");
    if (!f) return false;
    fclose(f);
    return true;
}

static void copy_text(char* dst, size_t dst_cap, const char* src) {
    if (!dst || dst_cap == 0) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    snprintf(dst, dst_cap, "%s", src);
}

static char* trim_in_place(char* text) {
    char* end;
    if (!text) return text;
    while (*text && isspace((unsigned char)*text)) ++text;
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) --end;
    *end = '\0';
    return text;
}

static bool parse_bool_value(const char* value, bool* out) {
    if (!value || !out) return false;
    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "yes") == 0 || strcmp(value, "on") == 0) {
        *out = true;
        return true;
    }
    if (strcmp(value, "0") == 0 || strcmp(value, "false") == 0 || strcmp(value, "no") == 0 || strcmp(value, "off") == 0) {
        *out = false;
        return true;
    }
    return false;
}

static bool parse_u16_value(const char* value, uint16_t* out) {
    unsigned long n;
    char* end;
    if (!value || !out) return false;
    n = strtoul(value, &end, 0);
    if (end == value || *trim_in_place(end) != '\0' || n > 65535ul) return false;
    *out = (uint16_t)n;
    return true;
}

static bool parse_u32_value(const char* value, uint32_t* out) {
    unsigned long n;
    char* end;
    if (!value || !out) return false;
    n = strtoul(value, &end, 0);
    if (end == value || *trim_in_place(end) != '\0') return false;
    *out = (uint32_t)n;
    return true;
}

static bool parse_float_value(const char* value, float* out) {
    double n;
    char* end;
    if (!value || !out) return false;
    n = strtod(value, &end);
    if (end == value || *trim_in_place(end) != '\0') return false;
    *out = (float)n;
    return true;
}

static bool parse_key_value(F4mpLiveTransportConfig* cfg, const char* section, char* line) {
    char* eq;
    char* key;
    char* value;
    bool b;
    uint16_t u16;
    uint32_t u32;
    float f;

    if (!cfg || !line) return false;
    eq = strchr(line, '=');
    if (!eq) return false;
    *eq = '\0';
    key = trim_in_place(line);
    value = trim_in_place(eq + 1);
    if (!key[0]) return false;

    if (section[0] == '\0' || strcmp(section, "live_transport") == 0) {
        if (strcmp(key, "enabled") == 0 || strcmp(key, "dev_enabled") == 0 || strcmp(key, "dev_live_transport") == 0) {
            if (parse_bool_value(value, &b)) cfg->dev_enabled = b;
            return true;
        }
        if (strcmp(key, "host") == 0 || strcmp(key, "host_ipv4") == 0) {
            copy_text(cfg->host_ipv4, sizeof(cfg->host_ipv4), value);
            return true;
        }
        if (strcmp(key, "port") == 0) {
            if (parse_u16_value(value, &u16)) cfg->port = u16;
            return true;
        }
        if (strcmp(key, "player_guid") == 0) {
            copy_text(cfg->player_guid, sizeof(cfg->player_guid), value);
            return true;
        }
        if (strcmp(key, "player_name") == 0) {
            copy_text(cfg->player_name, sizeof(cfg->player_name), value);
            return true;
        }
        if (strcmp(key, "build_hash") == 0) {
            if (parse_u32_value(value, &u32)) cfg->build_hash = u32;
            return true;
        }
    }

    if (strcmp(section, "bridge") == 0 || strcmp(section, "live_transport") == 0) {
        if (strcmp(key, "heartbeat_interval_ticks") == 0) {
            if (parse_u32_value(value, &u32)) cfg->bridge.heartbeat_interval_ticks = u32;
            return true;
        }
        if (strcmp(key, "auto_step_remote_backend") == 0) {
            if (parse_bool_value(value, &b)) cfg->bridge.auto_step_remote_backend = b;
            return true;
        }
        if (strcmp(key, "owns_remote_backend") == 0) {
            if (parse_bool_value(value, &b)) cfg->bridge.owns_remote_backend = b;
            return true;
        }
        if (strcmp(key, "remote_backend_step_count") == 0) {
            if (parse_u32_value(value, &u32)) cfg->bridge.remote_backend_step_count = u32;
            return true;
        }
        if (strcmp(key, "apply_scene_backend_interp_config") == 0) {
            if (parse_bool_value(value, &b)) cfg->bridge.apply_scene_backend_interp_config = b;
            return true;
        }
    }

    if (strcmp(section, "scene_interpolation") == 0 || strcmp(section, "bridge") == 0 || strcmp(section, "live_transport") == 0) {
        if (strcmp(key, "scene_interp_enabled") == 0 || strcmp(key, "enabled") == 0) {
            if (parse_bool_value(value, &b)) cfg->bridge.scene_interp_config.enabled = b;
            return true;
        }
        if (strcmp(key, "scene_interp_position_alpha") == 0 || strcmp(key, "position_alpha") == 0) {
            if (parse_float_value(value, &f)) cfg->bridge.scene_interp_config.position_alpha = f;
            return true;
        }
        if (strcmp(key, "scene_interp_rotation_alpha") == 0 || strcmp(key, "rotation_alpha") == 0) {
            if (parse_float_value(value, &f)) cfg->bridge.scene_interp_config.rotation_alpha = f;
            return true;
        }
    }

    return true;
}

void fltcfg_set_defaults(F4mpLiveTransportConfig* cfg) {
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
    cfg->dev_enabled = false;
    copy_text(cfg->host_ipv4, sizeof(cfg->host_ipv4), "127.0.0.1");
    cfg->port = 7777u;
    copy_text(cfg->player_guid, sizeof(cfg->player_guid), "dev-guid-local");
    copy_text(cfg->player_name, sizeof(cfg->player_name), "DevPlayer");
    cfg->build_hash = 0x12345678u;
    cfg->bridge.heartbeat_interval_ticks = 10u;
    cfg->bridge.auto_step_remote_backend = true;
    cfg->bridge.owns_remote_backend = true;
    cfg->bridge.remote_backend_step_count = 2u;
    cfg->bridge.apply_scene_backend_interp_config = false;
    cfg->bridge.scene_interp_config.enabled = true;
    cfg->bridge.scene_interp_config.position_alpha = 0.35f;
    cfg->bridge.scene_interp_config.rotation_alpha = 0.5f;
}

bool fltcfg_load_file(const char* path, F4mpLiveTransportConfig* out_cfg) {
    F4mpLiveTransportConfig cfg;
    FILE* f;
    char line[512];
    char section[64];

    if (!path || !out_cfg) return false;
    f = fopen(path, "rb");
    if (!f) return false;

    fltcfg_set_defaults(&cfg);
    memset(section, 0, sizeof(section));

    while (fgets(line, sizeof(line), f)) {
        char* text = trim_in_place(line);
        size_t n;
        if (!text[0] || text[0] == ';' || text[0] == '#') continue;
        n = strlen(text);
        if (n >= 2u && text[0] == '[' && text[n - 1u] == ']') {
            text[n - 1u] = '\0';
            copy_text(section, sizeof(section), trim_in_place(text + 1));
            continue;
        }
        (void)parse_key_value(&cfg, section, text);
    }

    fclose(f);
    *out_cfg = cfg;
    return true;
}

bool fltcfg_load_default(F4mpLiveTransportConfig* out_cfg, char* out_path, size_t out_path_cap) {
    static const char* kCandidates[] = {
        "f4mp_live.ini",
        "F4SE/f4mp_live.ini",
        "Data/F4SE/Plugins/f4mp_live.ini"
    };
    size_t i;
#ifdef _WIN32
    char env_path[F4MP_LIVE_CONFIG_PATH_CAP];
    DWORD env_len = GetEnvironmentVariableA("F4MP_LIVE_CONFIG", env_path, (DWORD)sizeof(env_path));
    if (env_len > 0 && env_len < sizeof(env_path) && fltcfg_load_file(env_path, out_cfg)) {
        if (out_path && out_path_cap) copy_text(out_path, out_path_cap, env_path);
        return true;
    }
#else
    const char* env_path = getenv("F4MP_LIVE_CONFIG");
    if (env_path && env_path[0] && fltcfg_load_file(env_path, out_cfg)) {
        if (out_path && out_path_cap) copy_text(out_path, out_path_cap, env_path);
        return true;
    }
#endif

    for (i = 0; i < sizeof(kCandidates) / sizeof(kCandidates[0]); ++i) {
        if (!file_exists(kCandidates[i])) continue;
        if (!fltcfg_load_file(kCandidates[i], out_cfg)) continue;
        if (out_path && out_path_cap) copy_text(out_path, out_path_cap, kCandidates[i]);
        return true;
    }

    fltcfg_set_defaults(out_cfg);
    if (out_path && out_path_cap) out_path[0] = '\0';
    return false;
}
