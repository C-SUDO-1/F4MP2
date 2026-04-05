#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"
#include "commonlibf4_local_player_provider_stub.h"
#include "fo4_proxy_actor_backend_stub.h"
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    MovementLiveClientBridgeConfig cfg;
    int i;
    LocalPlayerObservedState st;
    const Fo4ProxyBackendPlayerState* remote;
    proxy_runtime_init();
    assert(f4mp_spawn_service(&svc, "7784"));
    usleep(200000);

    assert(mlcb_open(&a, "127.0.0.1", 7784, "guid-ea", "AliceE", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7784, "guid-eb", "BobE", 0x1234u));
    cfg = mlcb_get_config(&a);
    cfg.owns_remote_backend = false;
    mlcb_set_config(&a, &cfg);
    cfg = mlcb_get_config(&b);
    cfg.owns_remote_backend = true;
    mlcb_set_config(&b, &cfg);

    for (i = 0; i < 80; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) && mlcb_local_player_id(&b)) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);

    /* Install engine-facing seams */
    lpo_init(mlcb_local_player_id(&a));
    clf4_lpo_stub_install(mlcb_local_player_id(&a));
    rpm_init(mlcb_local_player_id(&b));
    fo4_proxy_backend_stub_install();

    memset(&st, 0, sizeof(st));
    st.position.x = 31.0f;
    st.position.y = 17.0f;
    st.rotation.yaw = 15.0f;
    st.stance = STANCE_RUN;
    clf4_lpo_stub_feed(&st);

    for (i = 0; i < 80; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->update.position.x == 31.0f && remote->update.position.y == 17.0f) break;
        usleep(10000);
    }

    remote = fo4_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->update.position.x == 31.0f);
    assert(remote->update.position.y == 17.0f);
    assert(remote->update.rotation.yaw == 15.0f);
    assert(fo4_proxy_backend_stub_spawn_count() >= 1);
    assert(fo4_proxy_backend_stub_update_count() >= 1);

    clf4_lpo_stub_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
