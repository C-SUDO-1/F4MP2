#pragma once

#include <stdbool.h>
#include "hook_bringup_config.h"
#include "hook_bringup_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PluginStartupReport {
    HookBringupMode requested_mode;
    HookBringupStatus status;
    HookReadiness readiness;
    unsigned int fatal_block_count;
    unsigned int degradable_block_count;
    bool ready;
} PluginStartupReport;

PluginStartupReport plugin_startup_selfcheck(HookBringupMode mode);

#ifdef __cplusplus
}
#endif
