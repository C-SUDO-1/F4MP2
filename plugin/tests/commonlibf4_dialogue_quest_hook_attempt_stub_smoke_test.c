#include <assert.h>
#include "commonlibf4_dialogue_quest_hook_attempt_stub.h"
#include "commonlibf4_address_provider.h"

int main(void) {
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    HookAttemptResult r;
    CommonLibF4AddressProvider full = clf4_address_provider_make_fixed(true,true,true,true,true,true,true,true);
    CommonLibF4AddressProvider partial = clf4_address_provider_make_fixed(true,true,true,true,true,true,false,true);

    r = clf4_attempt_install_dialogue_quest_hook_stub(&ok, true, true);
    assert(r.state == HOOK_INSTALL_INSTALLED);

    r = clf4_attempt_install_dialogue_quest_hook_with_provider(&ok, &full, true, true);
    assert(r.state == HOOK_INSTALL_INSTALLED);
    assert(r.address_detail.required_address_count == 2u);
    assert(r.address_detail.resolved_address_count == 2u);
    assert(r.address_detail.patch_step_attempted);
    assert(r.address_detail.patch_step_succeeded);

    r = clf4_attempt_install_dialogue_quest_hook_with_provider(&ok, &partial, true, true);
    assert(r.state == HOOK_INSTALL_PARTIAL);
    assert(r.error == HOOK_INSTALL_ERR_ADDRESS_MISSING);
    assert(r.address_detail.resolved_address_count == 1u);

    r = clf4_attempt_install_dialogue_quest_hook_with_provider(&ok, &full, true, false);
    assert(r.state == HOOK_INSTALL_FAILED);
    assert(r.error == HOOK_INSTALL_ERR_PATCH_REJECTED);
    assert(r.address_detail.patch_step_attempted);
    assert(!r.address_detail.patch_step_succeeded);
    return 0;
}
