#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "test_service_process.h"
#include "movement_live_client_bridge.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    MovementLiveClientBridgeConfig cfg;
    bool sent = false, applied = false;
    int i;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* remote;
    proxy_runtime_init();
    assert(f4mp_spawn_service(&svc, "7796"));
    usleep(200000);

    assert(mlcb_open(&a, "127.0.0.1", 7796, "guid-own-a", "AliceOwn", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7796, "guid-own-b", "BobOwn", 0x1234u));
    cfg = mlcb_get_config(&a);
    cfg.owns_remote_backend = false;
    mlcb_set_config(&a, &cfg);

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) && mlcb_local_player_id(&b)) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);

    lpo_init(mlcb_local_player_id(&a));
    clf4_phob_install(mlcb_local_player_id(&a));
    rpm_init(mlcb_local_player_id(&b));
    fo4_scene_proxy_backend_stub_install();

    memset(&args, 0, sizeof(args));
    args.event.player_id = mlcb_local_player_id(&a);
    args.event.position.x = 5.0f;
    args.event.rotation.yaw = 5.0f;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 80; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->target_position.x == 5.0f) break;
        usleep(10000);
    }
    remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->interpolation_tick_count == 0);
    assert(remote->presented_position.x == 5.0f);

    clf4_phob_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
