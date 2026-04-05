#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_install_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HookReadiness {
    HOOK_READINESS_UNAVAILABLE = 0,
    HOOK_READINESS_PARTIAL = 1,
    HOOK_READINESS_READY = 2
} HookReadiness;

typedef struct HookBringupStatus {
    unsigned int installed_count;
    unsigned int attempted_count;
    unsigned int failed_count;
    unsigned int partial_count;
    unsigned int required_count;
    bool player_ready;
    bool actor_ready;
    bool workshop_ready;
    bool dialogue_quest_ready;
    bool core_runtime_ready;
    HookReadiness basic_sync_readiness;
    HookReadiness vanilla_mirror_readiness;
} HookBringupStatus;

void hook_bringup_status(HookBringupStatus* out_status);
bool hook_bringup_ready_for_basic_sync(void);
bool hook_bringup_ready_for_vanilla_mirror(void);
HookReadiness hook_bringup_basic_sync_readiness(void);
HookReadiness hook_bringup_vanilla_mirror_readiness(void);

#ifdef __cplusplus
}
#endif
