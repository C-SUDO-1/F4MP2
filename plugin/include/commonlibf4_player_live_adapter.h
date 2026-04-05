#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "commonlibf4_hook_adapter_stub.h"
#include "local_player_observer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4PlayerLiveSourceKind {
    CLF4_PLA_SOURCE_NONE = 0,
    CLF4_PLA_SOURCE_HOOK_CALLBACK = 1
} CommonLibF4PlayerLiveSourceKind;

typedef enum CommonLibF4PlayerLiveRejectReason {
    CLF4_PLA_REJECT_NONE = 0,
    CLF4_PLA_REJECT_NULL_ARGS = 1,
    CLF4_PLA_REJECT_NULL_OUT = 2,
    CLF4_PLA_REJECT_MISSING_PLAYER_ID = 3,
    CLF4_PLA_REJECT_INVALID_POSITION = 4,
    CLF4_PLA_REJECT_INVALID_ROTATION = 5,
    CLF4_PLA_REJECT_INVALID_VELOCITY = 6,
    CLF4_PLA_REJECT_INVALID_STANCE = 7
} CommonLibF4PlayerLiveRejectReason;

typedef struct CommonLibF4PlayerLiveSample {
    LocalPlayerObservedState state;
    uint32_t observed_tick;
    uint32_t accepted_sequence;
    uint8_t source_kind;
    bool used_default_player_id;
} CommonLibF4PlayerLiveSample;

typedef struct CommonLibF4PlayerLiveAdapterStats {
    PlayerId fallback_player_id;
    uint32_t normalize_attempt_count;
    uint32_t accept_count;
    uint32_t reject_count;
    uint32_t last_observed_tick;
    uint32_t last_accepted_sequence;
    bool has_last_sample;
    CommonLibF4PlayerLiveRejectReason last_reject_reason;
    CommonLibF4PlayerLiveSample last_sample;
} CommonLibF4PlayerLiveAdapterStats;

void clf4_pla_reset(PlayerId fallback_player_id);
bool clf4_pla_normalize(const CommonLibF4PlayerHookArgs* args, CommonLibF4PlayerLiveSample* out_sample);
CommonLibF4PlayerLiveAdapterStats clf4_pla_stats(void);

#ifdef __cplusplus
}
#endif
