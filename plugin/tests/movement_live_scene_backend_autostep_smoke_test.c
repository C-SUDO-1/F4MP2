#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "remote_proxy_manager.h"
#include "remote_proxy_update_policy.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    MovementLiveClientBridgeConfig bridge_cfg;
    int i;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* remote;
    assert(f4mp_spawn_service(&svc, "7793"));
    usleep(200000);

    assert(mlcb_open(&a, "127.0.0.1", 7793, "guid-as-a", "AliceAS", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7793, "guid-as-b", "BobAS", 0x1234u));

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
    Fo4SceneProxyInterpolationConfig cfg;
    RemoteProxyUpdatePolicy pol;
    rpm_init(mlcb_local_player_id(&b));
    fo4_scene_proxy_backend_stub_install();
    pol.enabled = true;
    pol.teleport_distance = 1000.0f;
    pol.position_lerp_alpha = 1.0f;
    pol.rotation_lerp_alpha = 1.0f;
    rpup_set(&pol);

    cfg.enabled = true;
    cfg.position_alpha = 0.1f;
    cfg.rotation_alpha = 0.1f;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&cfg);

    memset(&args, 0, sizeof(args));
    args.event.player_id = mlcb_local_player_id(&a);
    args.event.position.x = 100.0f;
    args.event.rotation.yaw = 90.0f;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->target_position.x == 100.0f && remote->interpolation_tick_count > 0) break;
        usleep(10000);
    }

    remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->target_position.x == 100.0f);
    assert(remote->interpolation_tick_count > 0);
    assert(!remote->last_update_teleported);
    assert(remote->presented_position.x >= 0.0f);

    clf4_phob_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
