#include "commonlibf4_dialogue_quest_hook_attempt_stub.h"

HookAttemptResult clf4_attempt_install_dialogue_quest_hook_stub(
    const F4SEInterfaceMock* f4se,
    bool observer_available,
    bool patch_allowed
) {
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_DEGRADABLE, true);
    }
    if (!observer_available) {
        return hook_attempt_result_make(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_DEPENDENCY_MISSING, HOOK_BLOCKING_DEGRADABLE, true);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_DEGRADABLE, true);
    }
    return hook_attempt_result_make(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true);
}

HookAttemptResult clf4_attempt_install_dialogue_quest_hook_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool observer_available,
    bool patch_allowed
) {
    uintptr_t addr = 0;
    bool ok_dialogue = clf4_address_provider_resolve(provider, CLF4_ADDR_DIALOGUE_OBSERVER, &addr);
    bool ok_quest = clf4_address_provider_resolve(provider, CLF4_ADDR_QUEST_OBSERVER, &addr);
    HookAddressAttemptDetail detail = hook_address_attempt_detail_make(
        (ok_dialogue ? 1u : 0u) + (ok_quest ? 1u : 0u),
        2u,
        true,
        patch_allowed
    );
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!observer_available) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_DEPENDENCY_MISSING, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!(ok_dialogue && ok_quest)) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    return hook_attempt_result_make_detail(HOOK_FAMILY_DIALOGUE_QUEST, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true, detail);
}
