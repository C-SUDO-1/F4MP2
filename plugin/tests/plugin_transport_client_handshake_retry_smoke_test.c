#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "plugin_transport_client.h"
#include "proxy_runtime.h"

int main(void) {
    PluginTransportClient c;
    PluginTransportHandshakeStats stats;
    F4mpServiceProcess svc;
    bool applied = false;
    int i;

    proxy_runtime_init();
    assert(ptc_open(&c, "127.0.0.1", 7797));
    assert(ptc_send_hello(&c, 0x1234u, "guid-retry-1", "retry-one"));

    for (i = 0; i < 8; ++i) {
        assert(ptc_poll_once(&c, 10, &applied));
        usleep(10000);
    }
    assert(!ptc_connected(&c));
    assert(f4mp_spawn_service(&svc, "7797"));
    usleep(200000);

    for (i = 0; i < 80; ++i) {
        assert(ptc_poll_once(&c, 10, &applied));
        if (ptc_connected(&c)) break;
        usleep(10000);
    }

    assert(ptc_connected(&c));
    assert(ptc_local_player_id(&c) != 0);
    stats = ptc_get_handshake_stats(&c);
    assert(stats.welcome_received);
    assert(stats.hello_send_count >= 2);
    assert(stats.hello_retry_count >= 1);

    ptc_close(&c);
    f4mp_stop_service(&svc);
    return 0;
}
