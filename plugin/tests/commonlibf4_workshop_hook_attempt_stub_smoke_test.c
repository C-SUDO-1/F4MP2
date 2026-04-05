#include <assert.h>
#include "commonlibf4_workshop_hook_attempt_stub.h"

int main(void) {
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    HookAttemptResult r = clf4_attempt_install_workshop_hook_stub(&ok, true, true);
    assert(r.state == HOOK_INSTALL_INSTALLED);

    r = clf4_attempt_install_workshop_hook_stub(&ok, false, true);
    assert(r.state == HOOK_INSTALL_PARTIAL);
    return 0;
}
