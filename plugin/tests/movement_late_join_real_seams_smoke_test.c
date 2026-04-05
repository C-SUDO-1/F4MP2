#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"
#include "plugin_transport_client.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a;
    PluginTransportClient b;
    bool sent = false, applied = false;
    bool late_applied = false;
    bool saw_send = false;
    int i;
    PlayerId remote_player_id;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* scene_remote;
    const Fo4ProxyActorState* driver_remote;
    const ProxyPlayerRecord* runtime_remote;
    CommonLibF4PlayerHookObserverBridgeStats bridge_stats;
    assert(f4mp_spawn_service(&svc, "7798"));
    usleep(200000);

    proxy_runtime_init();
    fpad_reset_default_driver();

    assert(mlcb_open(&a, "127.0.0.1", 7798, "guid-lrj-a", "AliceLRJ", 0x1234u));
    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        if (mlcb_local_player_id(&a) != 0) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    remote_player_id = mlcb_local_player_id(&a);

    lpo_init(remote_player_id);
    clf4_phob_install(remote_player_id);

    memset(&args, 0, sizeof(args));
    args.event.player_id = remote_player_id;
    args.event.position.x = 88.0f;
    args.event.position.y = 33.0f;
    args.event.position.z = 5.0f;
    args.event.velocity.x = 2.0f;
    args.event.velocity.y = 0.5f;
    args.event.rotation.yaw = 270.0f;
    args.event.stance = STANCE_WALK;
    args.event.anim_state = 9u;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        if (sent) {
            saw_send = true;
            break;
        }
        usleep(10000);
    }
    assert(saw_send);

    for (i = 0; i < 10; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        usleep(5000);
    }

    assert(ptc_open(&b, "127.0.0.1", 7798));
    assert(ptc_send_hello(&b, 0x1234u, "guid-lrj-b", "BobLRJ"));
    for (i = 0; i < 120; ++i) {
        assert(ptc_poll_once(&b, 10, &late_applied));
        assert(mlcb_tick(&a, &sent, &applied));
        if (ptc_connected(&b)) break;
        usleep(10000);
    }
    assert(ptc_connected(&b));
    assert(ptc_local_player_id(&b) != 0);

    proxy_runtime_init();
    fpad_reset_default_driver();
    rpm_init(ptc_local_player_id(&b));
    fo4_scene_proxy_backend_stub_install();

    for (i = 0; i < 160; ++i) {
        assert(ptc_poll_once(&b, 10, &late_applied));
        assert(mlcb_tick(&a, &sent, &applied));
        scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
        driver_remote = fpad_get_state(remote_player_id);
        runtime_remote = rpm_get_remote_player(remote_player_id);
        if (scene_remote && driver_remote && runtime_remote &&
            scene_remote->last_update.position.x == 88.0f &&
            driver_remote->position.x == 88.0f &&
            runtime_remote->position.x == 88.0f) {
            break;
        }
        usleep(10000);
    }

    bridge_stats = clf4_phob_stats();
    assert(bridge_stats.submit_count == 1u);
    assert(bridge_stats.live_accept_count == 1u);
    assert(bridge_stats.live_reject_count == 0u);
    assert(bridge_stats.last_state.player_id == remote_player_id);

    scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
    driver_remote = fpad_get_state(remote_player_id);
    runtime_remote = rpm_get_remote_player(remote_player_id);

    assert(scene_remote != 0);
    assert(driver_remote != 0);
    assert(runtime_remote != 0);

    assert(scene_remote->visible);
    assert(!scene_remote->despawned);
    assert(scene_remote->last_update.position.x == 88.0f);
    assert(scene_remote->last_update.position.y == 33.0f);
    assert(scene_remote->last_update.position.z == 5.0f);
    assert(scene_remote->last_update.rotation.yaw == 270.0f);
    assert(scene_remote->last_update.stance == STANCE_WALK);
    assert(scene_remote->last_update.anim_state == 9u);

    assert(driver_remote->visible);
    assert(!driver_remote->despawned);
    assert(driver_remote->create_count == 1u);
    assert(driver_remote->move_count >= 1u);
    assert(driver_remote->rotate_count >= 1u);
    assert(driver_remote->despawn_count == 0u);
    assert(driver_remote->position.x == 88.0f);
    assert(driver_remote->position.y == 33.0f);
    assert(driver_remote->position.z == 5.0f);
    assert(driver_remote->rotation.yaw == 270.0f);
    assert(driver_remote->stance == STANCE_WALK);
    assert(driver_remote->anim_state == 9u);

    assert(runtime_remote->position.x == 88.0f);
    assert(runtime_remote->position.y == 33.0f);
    assert(runtime_remote->position.z == 5.0f);
    assert(runtime_remote->rotation.yaw == 270.0f);
    assert(runtime_remote->stance == STANCE_WALK);
    assert(runtime_remote->anim_state == 9u);

    assert(fo4_scene_proxy_backend_stub_spawn_count() == 1u);
    assert(fo4_scene_proxy_backend_stub_update_count() >= 1u);
    assert(fo4_scene_proxy_backend_stub_despawn_count() == 0u);
    assert(fpad_create_count() == 1u);
    assert(fpad_move_count() >= 1u);
    assert(fpad_rotate_count() >= 1u);
    assert(fpad_despawn_count() == 0u);

    ptc_close(&b);
    clf4_phob_uninstall();
    mlcb_close(&a);
    f4mp_stop_service(&svc);
    return 0;
}
