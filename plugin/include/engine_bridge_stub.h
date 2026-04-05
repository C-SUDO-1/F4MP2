#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "proxy_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EngineBridgePlayerSnapshot {
    PlayerId player_id;
    Vec3f position;
    Rot3f rotation;
    Vec3f velocity;
    uint8_t stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
} EngineBridgePlayerSnapshot;

typedef struct EngineBridgeActorSnapshot {
    ActorNetId actor_net_id;
    uint32_t base_form_id;
    Vec3f position;
    Rot3f rotation;
    float health_norm;
    uint16_t actor_flags;
    bool is_dead;
} EngineBridgeActorSnapshot;

typedef struct EngineBridgeObjectSnapshot {
    ObjectNetId object_net_id;
    uint32_t form_id;
    Vec3f position;
    Rot3f rotation;
    uint16_t state_flags;
} EngineBridgeObjectSnapshot;

void ebstub_init(void);
void ebstub_shutdown(void);
bool ebstub_live_tick(bool* out_sent, bool* out_applied);
bool ebstub_live_enabled(void);
bool ebstub_live_connected(void);
const char* ebstub_live_config_path(void);
size_t ebstub_player_count(void);
size_t ebstub_actor_count(void);
size_t ebstub_object_count(void);
bool ebstub_get_player_snapshot(size_t index, EngineBridgePlayerSnapshot* out);
bool ebstub_get_actor_snapshot(size_t index, EngineBridgeActorSnapshot* out);
bool ebstub_get_object_snapshot(size_t index, EngineBridgeObjectSnapshot* out);
bool ebstub_has_active_dialogue(void);
const ProxyDialogueState* ebstub_get_dialogue(void);
const ProxyQuestState* ebstub_get_quest(void);
const ProxyProfileState* ebstub_get_profile(void);
const ProxyCombatState* ebstub_get_combat(void);

#ifdef __cplusplus
}
#endif
