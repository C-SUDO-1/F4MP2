#include "profile_store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include <sys/types.h>
#include <errno.h>

static int json_find_u32(const char* src, const char* key, uint32_t* out_value) {
    char pattern[64]; char* pos; unsigned value;
    if (!src || !key || !out_value) return 0;
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    pos = strstr((char*)src, pattern); if (!pos) return 0;
    pos = strchr(pos, ':'); if (!pos) return 0; pos++;
    while (*pos == ' ' || *pos == '\t') pos++;
    if (sscanf(pos, "%u", &value) != 1) return 0;
    *out_value = (uint32_t)value; return 1;
}

static int json_find_string(const char* src, const char* key, char* out, size_t out_cap) {
    char pattern[64]; char* pos; char* start; char* end; size_t len;
    if (!src || !key || !out || out_cap == 0) return 0;
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    pos = strstr((char*)src, pattern); if (!pos) return 0;
    pos = strchr(pos, ':'); if (!pos) return 0;
    start = strchr(pos, '"'); if (!start) return 0; start++;
    end = strchr(start, '"'); if (!end) return 0;
    len = (size_t)(end - start); if (len + 1 > out_cap) return 0;
    memcpy(out, start, len); out[len] = '\0'; return 1;
}

void profile_make_default(PlayerProfileData* profile, const char* player_guid, const char* display_name) {
    if (!profile) return; memset(profile, 0, sizeof(*profile));
    if (player_guid) snprintf(profile->player_guid, sizeof(profile->player_guid), "%s", player_guid);
    if (display_name) snprintf(profile->display_name, sizeof(profile->display_name), "%s", display_name);
    profile->level = 1; profile->xp = 0; profile->equipped_form_id = 0; profile->caps = 0;
    profile->str_stat = profile->per_stat = profile->end_stat = profile->cha_stat = 5;
    profile->int_stat = profile->agi_stat = profile->luk_stat = 5;
    profile->item_count = 2;
    profile->items[0].form_id = 0x0001u; profile->items[0].count = 1; profile->items[0].equipped = 1;
    profile->items[1].form_id = 0x0002u; profile->items[1].count = 20; profile->items[1].equipped = 0;
    profile->perk_count = 2;
    profile->perks[0].perk_form_id = 0x1001u; profile->perks[0].rank = 1;
    profile->perks[1].perk_form_id = 0x1002u; profile->perks[1].rank = 1;
    profile->sex = 0; profile->body_preset = 0;
}

bool profile_ensure_parent_dir(const char* path) {
    char dir[256]; const char* slash; size_t len;
    if (!path) return false; slash = strrchr(path, '/'); if (!slash) return true;
    len = (size_t)(slash - path); if (len >= sizeof(dir)) return false;
    memcpy(dir, path, len); dir[len] = '\0';
    #ifdef _WIN32
    if (_mkdir(dir) == 0) return true;
#else
    if (mkdir(dir, 0777) == 0) return true;
#endif
    return errno == EEXIST;
}

bool profile_load_json(const char* path, PlayerProfileData* out_profile) {
    FILE* f; long size; char* buf; uint32_t tmp; uint16_t i;
    if (!path || !out_profile) return false; memset(out_profile, 0, sizeof(*out_profile));
    f = fopen(path, "rb"); if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    size = ftell(f); if (size < 0) { fclose(f); return false; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return false; }
    buf = (char*)malloc((size_t)size + 1); if (!buf) { fclose(f); return false; }
    if (fread(buf, 1, (size_t)size, f) != (size_t)size) { free(buf); fclose(f); return false; }
    buf[size] = '\0'; fclose(f);

    json_find_string(buf, "player_guid", out_profile->player_guid, sizeof(out_profile->player_guid));
    json_find_string(buf, "display_name", out_profile->display_name, sizeof(out_profile->display_name));
    json_find_u32(buf, "level", &out_profile->level);
    json_find_u32(buf, "xp", &out_profile->xp);
    json_find_u32(buf, "equipped_form_id", &out_profile->equipped_form_id);
    json_find_u32(buf, "caps", &out_profile->caps);
    if (json_find_u32(buf, "str", &tmp)) out_profile->str_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "per", &tmp)) out_profile->per_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "end", &tmp)) out_profile->end_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "cha", &tmp)) out_profile->cha_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "int", &tmp)) out_profile->int_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "agi", &tmp)) out_profile->agi_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "luk", &tmp)) out_profile->luk_stat=(uint8_t)tmp;
    if (json_find_u32(buf, "sex", &tmp)) out_profile->sex=(uint8_t)tmp;
    json_find_u32(buf, "body_preset", &out_profile->body_preset);
    if (json_find_u32(buf, "item_count", &tmp)) out_profile->item_count=(uint16_t)((tmp>16)?16:tmp);
    for (i=0;i<out_profile->item_count;i++) {
        char key[64];
        snprintf(key,sizeof(key),"item_%u_form_id",(unsigned)i); json_find_u32(buf,key,&out_profile->items[i].form_id);
        snprintf(key,sizeof(key),"item_%u_count",(unsigned)i); json_find_u32(buf,key,&out_profile->items[i].count);
        snprintf(key,sizeof(key),"item_%u_equipped",(unsigned)i); if (json_find_u32(buf,key,&tmp)) out_profile->items[i].equipped=(uint8_t)(tmp?1:0);
    }
    if (json_find_u32(buf, "perk_count", &tmp)) out_profile->perk_count=(uint16_t)((tmp>16)?16:tmp);
    for (i=0;i<out_profile->perk_count;i++) {
        char key[64];
        snprintf(key,sizeof(key),"perk_%u_form_id",(unsigned)i); json_find_u32(buf,key,&out_profile->perks[i].perk_form_id);
        snprintf(key,sizeof(key),"perk_%u_rank",(unsigned)i); if (json_find_u32(buf,key,&tmp)) out_profile->perks[i].rank=(uint8_t)tmp;
    }
    free(buf); return out_profile->player_guid[0] != '\0';
}

bool profile_save_json(const char* path, const PlayerProfileData* profile) {
    FILE* f; uint16_t i;
    if (!path || !profile) return false; if (!profile_ensure_parent_dir(path)) return false;
    f = fopen(path, "wb"); if (!f) return false;
    fprintf(f,
        "{\n"
        "  \"version\": 1,\n"
        "  \"player_guid\": \"%s\",\n"
        "  \"display_name\": \"%s\",\n"
        "  \"level\": %u,\n"
        "  \"xp\": %u,\n"
        "  \"equipped_form_id\": %u,\n"
        "  \"caps\": %u,\n"
        "  \"special\": { \"str\": %u, \"per\": %u, \"end\": %u, \"cha\": %u, \"int\": %u, \"agi\": %u, \"luk\": %u },\n"
        "  \"sex\": %u,\n"
        "  \"body_preset\": %u,\n"
        "  \"item_count\": %u,\n"
        "  \"perk_count\": %u,\n",
        profile->player_guid, profile->display_name, profile->level, profile->xp,
        profile->equipped_form_id, profile->caps,
        profile->str_stat, profile->per_stat, profile->end_stat, profile->cha_stat,
        profile->int_stat, profile->agi_stat, profile->luk_stat,
        profile->sex, profile->body_preset,
        profile->item_count, profile->perk_count);
    for (i=0;i<profile->item_count && i<16;i++) {
        fprintf(f,"  \"item_%u_form_id\": %u,\n  \"item_%u_count\": %u,\n  \"item_%u_equipped\": %u,\n",
            (unsigned)i, profile->items[i].form_id,
            (unsigned)i, profile->items[i].count,
            (unsigned)i, profile->items[i].equipped ? 1u : 0u);
    }
    for (i=0;i<profile->perk_count && i<16;i++) {
        fprintf(f,"  \"perk_%u_form_id\": %u,\n  \"perk_%u_rank\": %u%s\n",
            (unsigned)i, profile->perks[i].perk_form_id,
            (unsigned)i, (unsigned)profile->perks[i].rank,
            (i+1<profile->perk_count) ? "," : "");
    }
    fprintf(f,"}\n");
    fclose(f); return true;
}

