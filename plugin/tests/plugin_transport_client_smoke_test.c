#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "plugin_transport_client.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    PluginTransportClient c;
    bool applied = false;
    int i;
    assert(f4mp_spawn_service(&svc, "7781"));
    usleep(200000);
    proxy_runtime_init();
    assert(ptc_open(&c, "127.0.0.1", 7781));
    assert(ptc_send_hello(&c, 0x1234u, "guid-ptc-1", "ptc-one"));
    for (i = 0; i < 40; ++i) {
        assert(ptc_poll_once(&c, 25, &applied));
        if (ptc_connected(&c)) break;
    }
    assert(ptc_connected(&c));
    assert(ptc_local_player_id(&c) != 0);
    ptc_close(&c);
    f4mp_stop_service(&svc);
    return 0;
}
