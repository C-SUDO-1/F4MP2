#include <assert.h>
#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_address_provider.h"
#include "commonlibf4_actor_hook_attempt_stub.h"
#include "commonlibf4_workshop_hook_attempt_stub.h"

int main(void) {
    F4SEInterfaceMock rt = { 0x010A03D8u, 0, 0 };
    HookAttemptResult r;

    {
        CommonLibF4AddressProvider ok = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
        r = clf4_attempt_install_actor_hook_with_provider(&rt, &ok, true, true);
        assert(r.state == HOOK_INSTALL_INSTALLED);
    }
    {
        CommonLibF4AddressProvider miss_actor = clf4_address_provider_make_fixed(true, true, true, false, true, true, true, true);
        r = clf4_attempt_install_actor_hook_with_provider(&rt, &miss_actor, true, true);
        assert(r.state == HOOK_INSTALL_PARTIAL);
    }
    {
        CommonLibF4AddressProvider ok = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
        r = clf4_attempt_install_workshop_hook_with_provider(&rt, &ok, true);
        assert(r.state == HOOK_INSTALL_INSTALLED);
    }
    {
        CommonLibF4AddressProvider miss_work = clf4_address_provider_make_fixed(true, true, true, true, true, false, true, true);
        r = clf4_attempt_install_workshop_hook_with_provider(&rt, &miss_work, true);
        assert(r.state == HOOK_INSTALL_PARTIAL);
    }
    return 0;
}
