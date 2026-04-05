#include <assert.h>
#include "hook_attempt_result.h"

int main(void) {
    HookAddressAttemptDetail d = hook_address_attempt_detail_make(1u, 2u, true, false);
    HookAttemptResult r = hook_attempt_result_make_detail(
        HOOK_FAMILY_PLAYER,
        HOOK_INSTALL_PARTIAL,
        HOOK_INSTALL_ERR_ADDRESS_MISSING,
        HOOK_BLOCKING_FATAL,
        true,
        d
    );
    assert(r.address_detail.resolved_address_count == 1u);
    assert(r.address_detail.required_address_count == 2u);
    assert(r.address_detail.patch_step_attempted);
    assert(!r.address_detail.patch_step_succeeded);
    return 0;
}
