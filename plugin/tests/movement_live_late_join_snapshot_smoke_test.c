#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "plugin_transport_client.h"
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    PluginTransportClient a, b;
    bool applied = false;
    int i;
    MsgPlayerState ps;
    const ProxyPlayerRecord* remote;
    assert(f4mp_spawn_service(&svc, "7796"));
    usleep(200000);

    proxy_runtime_init();
    assert(ptc_open(&a, "127.0.0.1", 7796));
    assert(ptc_send_hello(&a, 0x1234u, "guid-lja", "LateJoinA"));
    for (i = 0; i < 80; ++i) {
        assert(ptc_poll_once(&a, 10, &applied));
        if (ptc_connected(&a)) break;
        usleep(10000);
    }
    assert(ptc_connected(&a));

    memset(&ps, 0, sizeof(ps));
    ps.player_id = ptc_local_player_id(&a);
    ps.position.x = 42.0f;
    ps.position.y = 24.0f;
    ps.rotation.yaw = 90.0f;
    ps.stance = STANCE_RUN;
    assert(ptc_send_player_state(&a, &ps));
    /* let service ingest A's state */
    for (i = 0; i < 20; ++i) {
        assert(ptc_poll_once(&a, 5, &applied));
        usleep(5000);
    }

    assert(ptc_open(&b, "127.0.0.1", 7796));
    assert(ptc_send_hello(&b, 0x1234u, "guid-ljb", "LateJoinB"));
    for (i = 0; i < 80; ++i) {
        assert(ptc_poll_once(&b, 10, &applied));
        if (ptc_connected(&b)) break;
        usleep(10000);
    }
    assert(ptc_connected(&b));

    rpm_init(ptc_local_player_id(&b));
    for (i = 0; i < 120; ++i) {
        assert(ptc_poll_once(&b, 10, &applied));
        remote = rpm_get_remote_player(ptc_local_player_id(&a));
        if (remote && remote->position.x == 42.0f && remote->position.y == 24.0f) break;
        usleep(10000);
    }
    remote = rpm_get_remote_player(ptc_local_player_id(&a));
    assert(remote != NULL);
    assert(remote->position.x == 42.0f);
    assert(remote->position.y == 24.0f);
    assert(remote->rotation.yaw == 90.0f);

    ptc_close(&a);
    ptc_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
