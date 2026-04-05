#include <assert.h>
#include "commonlibf4_hook_install_simulator.h"
#include "commonlibf4_hook_install_registry.h"
#include "hook_bringup_policy.h"

int main(void) {
    HookInstallPlan plan = hook_install_plan_success_for_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    plan.dialogue_quest.state = HOOK_INSTALL_PARTIAL;
    plan.dialogue_quest.error = HOOK_INSTALL_ERR_ADDRESS_MISSING;

    hook_install_simulator_apply_plan(&plan, HOOK_BRINGUP_VANILLA_MIRROR);
    assert(hook_install_registry_get_state(HOOK_FAMILY_PLAYER) == HOOK_INSTALL_INSTALLED);
    assert(hook_install_registry_get_state(HOOK_FAMILY_DIALOGUE_QUEST) == HOOK_INSTALL_PARTIAL);
    assert(hook_bringup_vanilla_mirror_readiness() == HOOK_READINESS_PARTIAL);
    return 0;
}
