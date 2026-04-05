#include <assert.h>
#include "commonlibf4_hook_attempt_executor.h"
#include "commonlibf4_hook_install_registry.h"

int main(void) {
    HookInstallPlan plan = hook_install_plan_success_for_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    HookAttemptResult results[4];
    unsigned int count = 0;
    plan.actor.state = HOOK_INSTALL_PARTIAL;
    plan.actor.error = HOOK_INSTALL_ERR_ADDRESS_MISSING;
    plan.workshop.state = HOOK_INSTALL_FAILED;
    plan.workshop.error = HOOK_INSTALL_ERR_PATCH_REJECTED;

    assert(hook_attempt_executor_apply_mode_plan(HOOK_BRINGUP_VANILLA_MIRROR, &plan, results, 4, &count));
    assert(count == 4u);
    assert(hook_install_registry_get_state(HOOK_FAMILY_WORKSHOP) == HOOK_INSTALL_FAILED);
    assert(results[2].family == HOOK_FAMILY_WORKSHOP);
    assert(results[2].blocking == HOOK_BLOCKING_FATAL);
    assert(results[1].family == HOOK_FAMILY_ACTOR);
    assert(results[1].blocking == HOOK_BLOCKING_DEGRADABLE);
    return 0;
}
