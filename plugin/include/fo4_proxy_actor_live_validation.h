#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "messages_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fo4ProxyActorLiveValidationResult {
    PlayerId player_id;
    bool candidate_enabled;
    bool scene_present;
    bool scene_visible;
    bool scene_despawned;
    bool driver_present;
    bool driver_visible;
    bool driver_despawned;
    bool actor_handle_match;
    bool proxy_base_form_id_present;
    bool presented_position_match;
    bool presented_rotation_match;
    bool candidate_last_player_match;
    bool spawn_seen;
    bool move_seen;
    bool rotate_seen;
    bool despawn_seen;
    bool validation_passed;
    uint64_t scene_actor_handle;
    uint64_t driver_actor_handle;
    uint32_t proxy_base_form_id;
} Fo4ProxyActorLiveValidationResult;

void fpalv_reset(void);
bool fpalv_validate_present(PlayerId player_id, Fo4ProxyActorLiveValidationResult* out_result);
bool fpalv_validate_despawn(PlayerId player_id, Fo4ProxyActorLiveValidationResult* out_result);

#ifdef __cplusplus
}
#endif
