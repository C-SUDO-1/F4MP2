#include <assert.h>
#include "hook_bringup_policy.h"
#include "commonlibf4_hook_install_registry.h"

int main(void) {
    HookBringupStatus status;

    hook_install_registry_reset();
    hook_bringup_status(&status);
    assert(status.basic_sync_readiness == HOOK_READINESS_UNAVAILABLE);
    assert(status.vanilla_mirror_readiness == HOOK_READINESS_UNAVAILABLE);

    hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME);
    hook_bringup_status(&status);
    assert(status.basic_sync_readiness == HOOK_READINESS_PARTIAL);
    assert(status.failed_count == 1);

    hook_install_registry_install(HOOK_FAMILY_WORKSHOP);
    hook_bringup_status(&status);
    assert(status.basic_sync_readiness == HOOK_READINESS_PARTIAL);

    hook_install_registry_install(HOOK_FAMILY_PLAYER);
    hook_bringup_status(&status);
    assert(status.basic_sync_readiness == HOOK_READINESS_READY);

    hook_install_registry_mark_state(HOOK_FAMILY_ACTOR, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_PATCH_REJECTED);
    hook_install_registry_mark_state(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_DEPENDENCY_MISSING);
    hook_bringup_status(&status);
    assert(status.vanilla_mirror_readiness == HOOK_READINESS_PARTIAL);

    hook_install_registry_install(HOOK_FAMILY_ACTOR);
    hook_install_registry_install(HOOK_FAMILY_DIALOGUE_QUEST);
    hook_bringup_status(&status);
    assert(status.vanilla_mirror_readiness == HOOK_READINESS_READY);

    return 0;
}
