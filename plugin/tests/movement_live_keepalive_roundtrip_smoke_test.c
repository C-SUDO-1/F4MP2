#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"
#include "movement_live_client_bridge.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    int i;
    assert(f4mp_spawn_service(&svc, "7790"));
    usleep(200000);

    proxy_runtime_init();
    assert(mlcb_open(&a, "127.0.0.1", 7790, "guid-keepalive-a", "Alice", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7790, "guid-keepalive-b", "Bob", 0x1234u));

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        usleep(10000);
    }

    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);
    assert(a.client.heartbeats_sent > 0);
    assert(b.client.heartbeats_sent > 0);
    assert(a.client.heartbeats_received > 0);
    assert(b.client.heartbeats_received > 0);

    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
