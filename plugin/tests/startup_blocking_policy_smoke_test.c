#include <assert.h>
#include "startup_blocking_policy.h"
#include "commonlibf4_hook_install_registry.h"

int main(void) {
    hook_install_registry_reset();
    hook_install_registry_install(HOOK_FAMILY_PLAYER);
    hook_install_registry_mark_state(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED);
    StartupBlockingSummary s = startup_blocking_summary_for_mode(HOOK_BRINGUP_BASIC_SYNC);
    assert(s.fatal_blocked);
    assert(s.fatal_count >= 1u);
    assert(s.adjusted_readiness == HOOK_READINESS_UNAVAILABLE);

    hook_install_registry_reset();
    hook_install_registry_install(HOOK_FAMILY_PLAYER);
    hook_install_registry_install(HOOK_FAMILY_WORKSHOP);
    hook_install_registry_mark_state(HOOK_FAMILY_ACTOR, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING);
    hook_install_registry_mark_state(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_ADDRESS_MISSING);
    s = startup_blocking_summary_for_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    assert(!s.fatal_blocked);
    assert(s.degradable_count >= 1u);
    assert(s.adjusted_readiness == HOOK_READINESS_PARTIAL);
    return 0;
}
