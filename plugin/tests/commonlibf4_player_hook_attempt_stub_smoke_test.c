#include <assert.h>
#include "commonlibf4_player_hook_attempt_stub.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "commonlibf4_address_provider.h"

int main(void) {
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    HookAttemptResult r = clf4_attempt_install_player_hook_stub(&ok, true, true, clf4_phpp_default_armed());
    assert(r.state == HOOK_INSTALL_INSTALLED);
    assert(r.blocking == HOOK_BLOCKING_NONE);

    r = clf4_attempt_install_player_hook_stub(&ok, false, true, clf4_phpp_default_armed());
    assert(r.state == HOOK_INSTALL_PARTIAL);
    assert(r.error == HOOK_INSTALL_ERR_ADDRESS_MISSING);
    assert(r.blocking == HOOK_BLOCKING_FATAL);

    F4SEInterfaceMock old = { 0x010A0000u, 0, 0 };
    r = clf4_attempt_install_player_hook_stub(&old, true, true, clf4_phpp_default_armed());
    assert(r.state == HOOK_INSTALL_FAILED);
    assert(r.error == HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME);
        {
        CommonLibF4AddressProvider prov = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
        r = clf4_attempt_install_player_hook_with_provider(&ok, &prov, true, clf4_phpp_default_armed());
        assert(r.state == HOOK_INSTALL_INSTALLED);
    }
    {
        CommonLibF4AddressProvider prov = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
        r = clf4_attempt_install_player_hook_with_provider(&ok, &prov, true, clf4_phpp_default_armed());
        assert(r.state == HOOK_INSTALL_PARTIAL);
        assert(r.error == HOOK_INSTALL_ERR_ADDRESS_MISSING);
    }
    return 0;
}
