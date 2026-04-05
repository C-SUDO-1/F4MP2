#include <cassert>
#include "commonlibf4_hook_install_registry.h"

int main() {
    hook_install_registry_reset();
    assert(hook_install_registry_attempted_count() == 0);
    assert(hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME));
    assert(hook_install_registry_attempted_count() == 1);
    assert(hook_install_registry_failed_count() == 1);
    assert(hook_install_registry_get_state(HOOK_FAMILY_PLAYER) == HOOK_INSTALL_FAILED);
    assert(hook_install_registry_get_error(HOOK_FAMILY_PLAYER) == HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME);
    assert(hook_install_registry_mark_state(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING));
    assert(hook_install_registry_partial_count() == 1);
    assert(!hook_install_registry_is_installed(HOOK_FAMILY_WORKSHOP));
    assert(hook_install_registry_install(HOOK_FAMILY_WORKSHOP));
    assert(hook_install_registry_is_installed(HOOK_FAMILY_WORKSHOP));
    assert(hook_install_registry_get_error(HOOK_FAMILY_WORKSHOP) == HOOK_INSTALL_ERR_NONE);
    return 0;
}
