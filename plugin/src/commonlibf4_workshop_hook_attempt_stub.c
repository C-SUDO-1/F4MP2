#include "commonlibf4_workshop_hook_attempt_stub.h"

HookAttemptResult clf4_attempt_install_workshop_hook_stub(
    const F4SEInterfaceMock* f4se,
    bool address_available,
    bool patch_allowed
) {
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_FATAL, true);
    }
    if (!address_available) {
        return hook_attempt_result_make(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_FATAL, true);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_FATAL, true);
    }
    return hook_attempt_result_make(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true);
}

HookAttemptResult clf4_attempt_install_workshop_hook_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool patch_allowed
) {
    uintptr_t addr = 0;
    bool ok_place = clf4_address_provider_resolve(provider, CLF4_ADDR_WORKSHOP_PLACE, &addr);
    bool ok_scrap = clf4_address_provider_resolve(provider, CLF4_ADDR_WORKSHOP_SCRAP, &addr);
    HookAddressAttemptDetail detail = hook_address_attempt_detail_make(
        (ok_place ? 1u : 0u) + (ok_scrap ? 1u : 0u),
        2u,
        true,
        patch_allowed
    );
    if (!f4se || f4se->is_editor || f4se->runtime_version < 0x010A03D8u) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!(ok_place && ok_scrap)) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    if (!patch_allowed) {
        return hook_attempt_result_make_detail(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_DEGRADABLE, true, detail);
    }
    return hook_attempt_result_make_detail(HOOK_FAMILY_WORKSHOP, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true, detail);
}
