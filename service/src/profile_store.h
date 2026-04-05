#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct PlayerProfileItemData {
    uint32_t form_id;
    uint32_t count;
    uint8_t  equipped;
} PlayerProfileItemData;

typedef struct PlayerProfilePerkData {
    uint32_t perk_form_id;
    uint8_t  rank;
} PlayerProfilePerkData;

typedef struct PlayerProfileData {
    char player_guid[64];
    char display_name[64];
    uint32_t level;
    uint32_t xp;
    uint32_t equipped_form_id;
    uint32_t caps;
    uint8_t str_stat;
    uint8_t per_stat;
    uint8_t end_stat;
    uint8_t cha_stat;
    uint8_t int_stat;
    uint8_t agi_stat;
    uint8_t luk_stat;
    uint16_t item_count;
    PlayerProfileItemData items[16];
    uint16_t perk_count;
    PlayerProfilePerkData perks[16];
    uint8_t sex;
    uint32_t body_preset;
} PlayerProfileData;

bool profile_load_json(const char* path, PlayerProfileData* out_profile);
bool profile_save_json(const char* path, const PlayerProfileData* profile);

void profile_make_default(PlayerProfileData* profile, const char* player_guid, const char* display_name);
bool profile_ensure_parent_dir(const char* path);
