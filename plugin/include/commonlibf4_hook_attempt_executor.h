#pragma once

#include "commonlibf4_hook_install_simulator.h"
#include "commonlibf4_entrypoint_stub.h"
#include "hook_attempt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

bool hook_attempt_executor_apply_mode_plan(
    HookBringupMode mode,
    const HookInstallPlan* plan,
    HookAttemptResult* out_results,
    unsigned int max_results,
    unsigned int* out_count
);

bool hook_attempt_executor_apply_mode_runtime_stub(
    const F4SEInterfaceMock* f4se,
    HookBringupMode mode,
    HookAttemptResult* out_results,
    unsigned int max_results,
    unsigned int* out_count
);

#ifdef __cplusplus
}
#endif
