#include "hook_attempt_result.h"

HookAddressAttemptDetail hook_address_attempt_detail_make(
    unsigned int resolved_address_count,
    unsigned int required_address_count,
    bool patch_step_attempted,
    bool patch_step_succeeded
) {
    HookAddressAttemptDetail d;
    d.resolved_address_count = resolved_address_count;
    d.required_address_count = required_address_count;
    d.patch_step_attempted = patch_step_attempted;
    d.patch_step_succeeded = patch_step_succeeded;
    return d;
}

HookAttemptResult hook_attempt_result_make(
    HookFamily family,
    HookInstallState state,
    HookInstallError error,
    HookInstallBlocking blocking,
    bool attempted
) {
    return hook_attempt_result_make_detail(
        family, state, error, blocking, attempted,
        hook_address_attempt_detail_make(0, 0, false, false)
    );
}

HookAttemptResult hook_attempt_result_make_detail(
    HookFamily family,
    HookInstallState state,
    HookInstallError error,
    HookInstallBlocking blocking,
    bool attempted,
    HookAddressAttemptDetail detail
) {
    HookAttemptResult r;
    r.family = family;
    r.state = state;
    r.error = error;
    r.blocking = blocking;
    r.attempted = attempted;
    r.address_detail = detail;
    return r;
}
