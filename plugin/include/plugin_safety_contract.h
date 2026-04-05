#pragma once

#include <stdint.h>
#include "protocol_types.h"

typedef enum SafetyAction { SAFETY_ALLOW=0, SAFETY_BLOCK=1, SAFETY_HOST_ONLY=2 } SafetyAction;

typedef struct SafetyEvaluation {
    SafetyAction action;
    uint16_t     reason_code;
} SafetyEvaluation;

SafetyEvaluation safety_can_guest_mutate_world(PlayerId player_id);
SafetyEvaluation safety_can_guest_start_dialogue(PlayerId player_id);
SafetyEvaluation safety_can_guest_trigger_quest_change(PlayerId player_id);
SafetyEvaluation safety_can_guest_place_object(PlayerId player_id, uint32_t form_id);
