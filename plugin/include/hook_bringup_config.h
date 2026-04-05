#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_install_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HookBringupMode {
    HOOK_BRINGUP_NONE = 0,
    HOOK_BRINGUP_BASIC_SYNC = 1,
    HOOK_BRINGUP_VANILLA_MIRROR = 2
} HookBringupMode;

typedef struct HookBringupConfig {
    HookBringupMode mode;
    bool enable_player;
    bool enable_actor;
    bool enable_workshop;
    bool enable_dialogue_quest;
} HookBringupConfig;

HookBringupConfig hook_bringup_config_for_mode(HookBringupMode mode);
void hook_bringup_apply_config(const HookBringupConfig* cfg);
void hook_bringup_apply_mode(HookBringupMode mode);

#ifdef __cplusplus
}
#endif
