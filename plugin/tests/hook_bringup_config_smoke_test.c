#include <assert.h>
#include <stdio.h>
#include "hook_bringup_config.h"
#include "commonlibf4_hook_install_registry.h"

int main(void) {
    hook_bringup_apply_mode(HOOK_BRINGUP_NONE);
    assert(hook_install_registry_installed_count() == 0);

    hook_bringup_apply_mode(HOOK_BRINGUP_BASIC_SYNC);
    assert(hook_install_registry_is_installed(HOOK_FAMILY_PLAYER));
    assert(!hook_install_registry_is_installed(HOOK_FAMILY_ACTOR));
    assert(hook_install_registry_is_installed(HOOK_FAMILY_WORKSHOP));
    assert(!hook_install_registry_is_installed(HOOK_FAMILY_DIALOGUE_QUEST));
    assert(hook_install_registry_installed_count() == 2);

    hook_bringup_apply_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    assert(hook_install_registry_is_installed(HOOK_FAMILY_PLAYER));
    assert(hook_install_registry_is_installed(HOOK_FAMILY_ACTOR));
    assert(hook_install_registry_is_installed(HOOK_FAMILY_WORKSHOP));
    assert(hook_install_registry_is_installed(HOOK_FAMILY_DIALOGUE_QUEST));
    assert(hook_install_registry_installed_count() == 4);

    puts("hook_bringup_config_smoke_test: ok");
    return 0;
}
