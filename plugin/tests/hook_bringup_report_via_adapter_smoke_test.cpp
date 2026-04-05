#include <cassert>
#include "commonlibf4_hook_adapter_stub.h"
#include "commonlibf4_hook_install_simulator.h"

int main() {
    HookInstallPlan plan = hook_install_plan_success_for_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    plan.actor.state = HOOK_INSTALL_PARTIAL;
    plan.actor.error = HOOK_INSTALL_ERR_ADDRESS_MISSING;
    clf4_hook_adapter_init_mode_with_plan(HOOK_BRINGUP_VANILLA_MIRROR, &plan);
    assert(clf4_hook_adapter_get_last_bringup_result_count() == HOOK_FAMILY_COUNT);
    HookAttemptResult r{};
    bool found_partial_actor = false;
    for (unsigned int i = 0; i < clf4_hook_adapter_get_last_bringup_result_count(); ++i) {
        assert(clf4_hook_adapter_get_last_bringup_result(i, &r));
        if (r.family == HOOK_FAMILY_ACTOR) {
            found_partial_actor = true;
            assert(r.state == HOOK_INSTALL_PARTIAL);
        }
    }
    assert(found_partial_actor);
    return 0;
}
