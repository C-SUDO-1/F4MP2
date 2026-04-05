#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_install_registry.h"
#include "hook_bringup_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HookInstallBlocking {
    HOOK_BLOCKING_NONE = 0,
    HOOK_BLOCKING_DEGRADABLE = 1,
    HOOK_BLOCKING_FATAL = 2
} HookInstallBlocking;

typedef struct HookAddressAttemptDetail {
    unsigned int resolved_address_count;
    unsigned int required_address_count;
    bool patch_step_attempted;
    bool patch_step_succeeded;
} HookAddressAttemptDetail;

typedef struct HookAttemptResult {
    HookFamily family;
    HookInstallState state;
    HookInstallError error;
    HookInstallBlocking blocking;
    bool attempted;
    HookAddressAttemptDetail address_detail;
} HookAttemptResult;

HookAddressAttemptDetail hook_address_attempt_detail_make(
    unsigned int resolved_address_count,
    unsigned int required_address_count,
    bool patch_step_attempted,
    bool patch_step_succeeded
);

HookAttemptResult hook_attempt_result_make(
    HookFamily family,
    HookInstallState state,
    HookInstallError error,
    HookInstallBlocking blocking,
    bool attempted
);

HookAttemptResult hook_attempt_result_make_detail(
    HookFamily family,
    HookInstallState state,
    HookInstallError error,
    HookInstallBlocking blocking,
    bool attempted,
    HookAddressAttemptDetail detail
);

#ifdef __cplusplus
}
#endif
