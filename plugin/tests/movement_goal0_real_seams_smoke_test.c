#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    int i;
    CommonLibF4PlayerHookArgs args;
    const Fo4SceneProxyPlayerState* scene_remote;
    const Fo4ProxyActorState* driver_remote;
    const ProxyPlayerRecord* runtime_remote;
    CommonLibF4PlayerHookObserverBridgeStats bridge_stats;
    PlayerId remote_player_id;
    assert(f4mp_spawn_service(&svc, "7794"));
    usleep(200000);

    proxy_runtime_init();
    fpad_reset_default_driver();

    assert(mlcb_open(&a, "127.0.0.1", 7794, "guid-rs-a", "AliceRS", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7794, "guid-rs-b", "BobRS", 0x1234u));

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) && mlcb_local_player_id(&b)) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);
    remote_player_id = mlcb_local_player_id(&a);

    lpo_init(remote_player_id);
    clf4_phob_install(remote_player_id);
    rpm_init(mlcb_local_player_id(&b));
    fo4_scene_proxy_backend_stub_install();

    memset(&args, 0, sizeof(args));
    args.event.player_id = remote_player_id;
    args.event.position.x = 42.0f;
    args.event.position.y = 24.0f;
    args.event.position.z = 6.0f;
    args.event.velocity.x = 1.5f;
    args.event.velocity.y = 0.25f;
    args.event.rotation.yaw = 135.0f;
    args.event.stance = STANCE_RUN;
    args.event.anim_state = 7u;
    assert(clf4_phob_submit(&args));

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
        driver_remote = fpad_get_state(remote_player_id);
        runtime_remote = rpm_get_remote_player(remote_player_id);
        if (scene_remote && driver_remote && runtime_remote &&
            scene_remote->last_update.position.x == 42.0f &&
            driver_remote->position.x == 42.0f &&
            runtime_remote->position.x == 42.0f) {
            break;
        }
        usleep(10000);
    }

    bridge_stats = clf4_phob_stats();
    assert(bridge_stats.submit_count == 1);
    assert(bridge_stats.live_accept_count == 1);
    assert(bridge_stats.live_reject_count == 0);
    assert(bridge_stats.last_state.player_id == remote_player_id);

    scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
    driver_remote = fpad_get_state(remote_player_id);
    runtime_remote = rpm_get_remote_player(remote_player_id);
    assert(scene_remote != 0);
    assert(driver_remote != 0);
    assert(runtime_remote != 0);

    assert(scene_remote->visible);
    assert(scene_remote->last_update.position.x == 42.0f);
    assert(scene_remote->last_update.position.y == 24.0f);
    assert(scene_remote->last_update.rotation.yaw == 135.0f);
    assert(scene_remote->last_update.stance == STANCE_RUN);
    assert(scene_remote->last_update.anim_state == 7u);

    assert(driver_remote->visible);
    assert(driver_remote->create_count == 1u);
    assert(driver_remote->move_count >= 1u);
    assert(driver_remote->rotate_count >= 1u);
    assert(driver_remote->position.x == 42.0f);
    assert(driver_remote->position.y == 24.0f);
    assert(driver_remote->rotation.yaw == 135.0f);
    assert(driver_remote->stance == STANCE_RUN);
    assert(driver_remote->anim_state == 7u);

    assert(runtime_remote->position.x == 42.0f);
    assert(runtime_remote->rotation.yaw == 135.0f);
    assert(runtime_remote->stance == STANCE_RUN);
    assert(fpad_create_count() == 1u);
    assert(fpad_move_count() >= 1u);
    assert(fpad_rotate_count() >= 1u);
    assert(fpad_despawn_count() == 0u);

    mlcb_close(&a);

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&b, &sent, &applied));
        scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
        driver_remote = fpad_get_state(remote_player_id);
        runtime_remote = rpm_get_remote_player(remote_player_id);
        if (runtime_remote == 0 && scene_remote && scene_remote->despawned && driver_remote && driver_remote->despawned) {
            break;
        }
        usleep(10000);
    }

    scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id);
    driver_remote = fpad_get_state(remote_player_id);
    assert(rpm_get_remote_player(remote_player_id) == 0);
    assert(scene_remote != 0);
    assert(scene_remote->despawned);
    assert(!scene_remote->visible);
    assert(driver_remote != 0);
    assert(driver_remote->despawned);
    assert(!driver_remote->visible);
    assert(driver_remote->despawn_count == 1u);
    assert(fpad_despawn_count() == 1u);

    clf4_phob_uninstall();
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
