#include "commonlibf4_actor_hook_attempt_stub.h"

HookAttemptResult clf4_attempt_install_actor_hook_stub(
    const F4SEInterfaceMock* f4se,
    bool address_available,
    bool dependency_available,
    bool patch_allowed
) {
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_DEGRADABLE, true);
    }
    if (!dependency_available) {
        return hook_attempt_result_make(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_DEPENDENCY_MISSING, HOOK_BLOCKING_DEGRADABLE, true);
    }
    if (!address_available) {
        return hook_attempt_result_make(HOOK_FAMILY_ACTOR, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_DEGRADABLE, true);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_DEGRADABLE, true);
    }
    return hook_attempt_result_make(HOOK_FAMILY_ACTOR, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true);
}

HookAttemptResult clf4_attempt_install_actor_hook_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool dependency_available,
    bool patch_allowed
) {
    uintptr_t addr = 0;
    bool ok_update = clf4_address_provider_resolve(provider, CLF4_ADDR_ACTOR_UPDATE, &addr);
    bool ok_damage = clf4_address_provider_resolve(provider, CLF4_ADDR_ACTOR_DAMAGE, &addr);
    HookAddressAttemptDetail detail = hook_address_attempt_detail_make(
        (ok_update ? 1u : 0u) + (ok_damage ? 1u : 0u),
        2u,
        true,
        patch_allowed
    );
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!dependency_available) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_DEPENDENCY_MISSING, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!(ok_update && ok_damage)) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_ACTOR, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_ACTOR, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    return hook_attempt_result_make_detail(HOOK_FAMILY_ACTOR, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true, detail);
}
