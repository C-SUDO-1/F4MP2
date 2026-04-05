#pragma once

#include "hook_attempt_result.h"
#include "hook_bringup_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StartupBlockingSummary {
    unsigned int fatal_count;
    unsigned int degradable_count;
    bool fatal_blocked;
    HookReadiness adjusted_readiness;
} StartupBlockingSummary;

HookInstallBlocking startup_blocking_for_family(
    HookBringupMode mode,
    HookFamily family,
    HookInstallState state,
    HookInstallError error
);

StartupBlockingSummary startup_blocking_summary_for_mode(HookBringupMode mode);

#ifdef __cplusplus
}
#endif
