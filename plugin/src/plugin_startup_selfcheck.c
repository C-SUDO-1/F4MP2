#include "plugin_startup_selfcheck.h"
#include "startup_blocking_policy.h"

PluginStartupReport plugin_startup_selfcheck(HookBringupMode mode) {
    PluginStartupReport report;
    StartupBlockingSummary blocking;
    report.requested_mode = mode;
    hook_bringup_status(&report.status);

    switch (mode) {
        case HOOK_BRINGUP_BASIC_SYNC:
            report.readiness = hook_bringup_basic_sync_readiness();
            break;
        case HOOK_BRINGUP_VANILLA_MIRROR:
            report.readiness = hook_bringup_vanilla_mirror_readiness();
            break;
        case HOOK_BRINGUP_NONE:
        default:
            report.readiness = HOOK_READINESS_UNAVAILABLE;
            break;
    }

    blocking = startup_blocking_summary_for_mode(mode);
    report.readiness = blocking.adjusted_readiness;
    report.fatal_block_count = blocking.fatal_count;
    report.degradable_block_count = blocking.degradable_count;
    report.ready = report.readiness == HOOK_READINESS_READY;
    return report;
}
