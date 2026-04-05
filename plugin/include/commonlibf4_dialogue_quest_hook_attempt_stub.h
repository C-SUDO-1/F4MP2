#pragma once

#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_address_provider.h"
#include "hook_attempt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

HookAttemptResult clf4_attempt_install_dialogue_quest_hook_stub(
    const F4SEInterfaceMock* f4se,
    bool observer_available,
    bool patch_allowed
);

HookAttemptResult clf4_attempt_install_dialogue_quest_hook_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool observer_available,
    bool patch_allowed
);

#ifdef __cplusplus
}
#endif
