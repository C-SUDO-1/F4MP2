#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_player.h"

typedef struct PlayerHookShimStats {
    uint32_t ingest_count;
    uint32_t real_entry_count;
    uint32_t detour_entry_count;
    uint32_t detour_bridge_forward_count;
    uint32_t real_dispatch_attempt_count;
    uint32_t real_dispatch_success_count;
    uint32_t real_dispatch_failure_count;
    uint32_t last_thread_id;
    uintptr_t last_return_address;
    bool has_last_return_address;
} PlayerHookShimStats;

void phs_init(void);
void phs_note_manual_detour_entry(const char* site_label);
void phs_set_manual_create_player_controls_trampoline(uintptr_t trampoline_addr);
uintptr_t phs_get_manual_create_player_controls_trampoline(void);
uintptr_t phs_create_player_controls_manual_detour_thunk(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4);
bool phs_ingest_player_snapshot(const MsgPlayerState* msg);
bool phs_ingest_real_player_snapshot(const MsgPlayerState* msg);
bool phs_get_last_player_snapshot(MsgPlayerState* out_msg);
PlayerHookShimStats phs_stats(void);
