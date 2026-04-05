#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_install_registry.h"
#include "hook_bringup_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HookInstallPlan {
    HookInstallRecord player;
    HookInstallRecord actor;
    HookInstallRecord workshop;
    HookInstallRecord dialogue_quest;
} HookInstallPlan;

HookInstallPlan hook_install_plan_success_for_mode(HookBringupMode mode);
void hook_install_simulator_apply_plan(const HookInstallPlan* plan, HookBringupMode mode);

#ifdef __cplusplus
}
#endif
