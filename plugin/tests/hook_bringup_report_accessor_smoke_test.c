#include <assert.h>
#include "hook_bringup_report_accessor.h"

int main(void) {
    HookAttemptResult r[2];
    HookAttemptResult out;
    hook_bringup_report_reset();
    assert(!hook_bringup_report_has_value());
    r[0] = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true);
    r[1] = hook_attempt_result_make(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_FATAL, true);
    hook_bringup_report_store(r, 2);
    assert(hook_bringup_report_has_value());
    assert(hook_bringup_report_count() == 2);
    assert(hook_bringup_report_get_result(1, &out));
    assert(out.family == HOOK_FAMILY_WORKSHOP);
    assert(out.state == HOOK_INSTALL_FAILED);
    assert(!hook_bringup_report_get_result(2, &out));
    return 0;
}
