#include <assert.h>
#include "commonlibf4_actor_hook_attempt_stub.h"

int main(void) {
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    HookAttemptResult r = clf4_attempt_install_actor_hook_stub(&ok, true, true, true);
    assert(r.state == HOOK_INSTALL_INSTALLED);

    r = clf4_attempt_install_actor_hook_stub(&ok, true, false, true);
    assert(r.state == HOOK_INSTALL_FAILED);
    assert(r.error == HOOK_INSTALL_ERR_DEPENDENCY_MISSING);
    return 0;
}
