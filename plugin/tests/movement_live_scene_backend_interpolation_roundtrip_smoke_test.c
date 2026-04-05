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
    MovementLiveClientBridgeConfig mlcfg_a;
    int i;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* remote;
    MovementLiveClientBridgeConfig mlcfg;
    Fo4SceneProxyInterpolationConfig cfg;
    RemoteProxyUpdatePolicy pol;
    assert(f4mp_spawn_service(&svc, "7792"));
    usleep(200000);

    assert(mlcb_open(&a, "127.0.0.1", 7792, "guid-int-a", "AliceInt", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7792, "guid-int-b", "BobInt", 0x1234u));
    mlcfg_a = mlcb_get_config(&a);
    mlcfg_a.owns_remote_backend = false;
    mlcb_set_config(&a, &mlcfg_a);

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) && mlcb_local_player_id(&b)) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);

    mlcfg = mlcb_get_config(&b);
    mlcfg.auto_step_remote_backend = false;
    mlcb_set_config(&b, &mlcfg);

    lpo_init(mlcb_local_player_id(&a));
    clf4_phob_install(mlcb_local_player_id(&a));
    rpm_init(mlcb_local_player_id(&b));
    fo4_scene_proxy_backend_stub_install();
    pol.enabled = true;
    pol.teleport_distance = 1000.0f;
    pol.position_lerp_alpha = 1.0f;
    pol.rotation_lerp_alpha = 1.0f;
    rpup_set(&pol);

    cfg.enabled = true;
    cfg.position_alpha = 0.5f;
    cfg.rotation_alpha = 0.5f;
    fo4_scene_proxy_backend_stub_set_interpolation_config(&cfg);

    memset(&args, 0, sizeof(args));
    args.event.player_id = mlcb_local_player_id(&a);
    args.event.position.x = 5.0f;
    args.event.rotation.yaw = 5.0f;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->target_position.x == 5.0f) break;
        usleep(10000);
    }

    memset(&args, 0, sizeof(args));
    args.event.player_id = mlcb_local_player_id(&a);
    args.event.position.x = 20.0f;
    args.event.rotation.yaw = 30.0f;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->target_position.x == 20.0f) break;
        usleep(10000);
    }
    remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->interpolation_pending);
    assert(remote->presented_position.x == 5.0f);
    assert(remote->target_position.x == 20.0f);

    fo4_scene_proxy_backend_stub_step();
    remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->interpolation_tick_count >= 1);
    assert(remote->presented_position.x > 5.0f);
    assert(remote->presented_position.x < remote->target_position.x);

    clf4_phob_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
