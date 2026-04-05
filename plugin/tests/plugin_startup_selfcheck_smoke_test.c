#include <assert.h>
#include "plugin_startup_selfcheck.h"
#include "commonlibf4_hook_install_registry.h"

int main(void) {
    PluginStartupReport report;

    hook_install_registry_reset();
    report = plugin_startup_selfcheck(HOOK_BRINGUP_BASIC_SYNC);
    assert(report.readiness == HOOK_READINESS_UNAVAILABLE);
    assert(!report.ready);

    hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING);
    report = plugin_startup_selfcheck(HOOK_BRINGUP_BASIC_SYNC);
    assert(report.readiness == HOOK_READINESS_UNAVAILABLE);
    assert(report.fatal_block_count >= 1u);
    assert(!report.ready);

    hook_install_registry_install(HOOK_FAMILY_PLAYER);
    hook_install_registry_install(HOOK_FAMILY_WORKSHOP);
    report = plugin_startup_selfcheck(HOOK_BRINGUP_BASIC_SYNC);
    assert(report.readiness == HOOK_READINESS_READY);
    assert(report.ready);

    hook_install_registry_install(HOOK_FAMILY_DIALOGUE_QUEST);
    hook_install_registry_mark_state(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED);
    report = plugin_startup_selfcheck(HOOK_BRINGUP_VANILLA_MIRROR);
    assert(report.readiness == HOOK_READINESS_PARTIAL);
    assert(report.degradable_block_count >= 1u);
    assert(!report.ready);

    return 0;
}
