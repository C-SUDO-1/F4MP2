#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "protocol_types.h"
#include "messages_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LocalPlayerObservedState {
    bool     valid;
    PlayerId player_id;
    Vec3f    position;
    Rot3f    rotation;
    Vec3f    velocity;
    uint8_t  stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
} LocalPlayerObservedState;

void lpo_init(PlayerId local_player_id);
void lpo_set_synthetic_state(const LocalPlayerObservedState* state);
bool lpo_capture(LocalPlayerObservedState* out_state);
static inline bool lpo_capture_state(LocalPlayerObservedState* out_state) { return lpo_capture(out_state); }
PlayerId lpo_local_player_id(void);

#ifdef __cplusplus
}
#endif
