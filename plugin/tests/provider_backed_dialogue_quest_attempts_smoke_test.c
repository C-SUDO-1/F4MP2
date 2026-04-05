#include <assert.h>
#include "commonlibf4_hook_attempt_executor.h"

int main(void) {
    F4SEInterfaceMock runtime = { 0x010A03D8u, 0, 0 };
    HookAttemptResult results[HOOK_FAMILY_COUNT];
    unsigned int count = 0;
    bool ok = hook_attempt_executor_apply_mode_runtime_stub(&runtime, HOOK_BRINGUP_VANILLA_MIRROR, results, HOOK_FAMILY_COUNT, &count);
    assert(ok);
    assert(count == HOOK_FAMILY_COUNT);
    HookAttemptResult dq = results[HOOK_FAMILY_DIALOGUE_QUEST];
    assert(dq.attempted);
    assert(dq.address_detail.required_address_count == 2u);
    assert(dq.address_detail.resolved_address_count == 2u);
    return 0;
}
