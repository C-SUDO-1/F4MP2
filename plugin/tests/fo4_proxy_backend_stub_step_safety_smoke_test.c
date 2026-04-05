#include <assert.h>

#include "fo4_proxy_actor_backend_stub.h"
#include "remote_proxy_backend.h"

int main(void) {
    fo4_proxy_backend_stub_install();
    /* Should be safe even when the stub backend does not provide an active step implementation. */
    rpb_step_backend();
    rpb_step_backend();
    return 0;
}
