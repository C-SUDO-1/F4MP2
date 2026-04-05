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

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    int i;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* remote;
    assert(f4mp_spawn_service(&svc, "7791"));
    usleep(200000);

    assert(mlcb_open(&a, "127.0.0.1", 7791, "guid-hf-a", "AliceHF", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7791, "guid-hf-b", "BobHF", 0x1234u));

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
    args.event.position.x = 55.0f;
    args.event.position.y = 12.0f;
    args.event.rotation.yaw = 22.0f;
    args.event.stance = STANCE_WALK;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 100; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
        if (remote && remote->last_update.position.x == 55.0f && remote->last_update.position.y == 12.0f) break;
        usleep(10000);
    }

    remote = fo4_scene_proxy_backend_stub_get(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->visible);
    assert(remote->last_update.position.x == 55.0f);
    assert(remote->last_update.position.y == 12.0f);
    assert(remote->last_update.rotation.yaw == 22.0f);

    clf4_phob_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
