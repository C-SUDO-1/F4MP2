#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"
#include "local_player_observer.h"
#include "local_player_observer_provider.h"
#include "remote_proxy_manager.h"

int main(void) {
    F4mpServiceProcess svc;
    MovementLiveClientBridge a, b;
    bool sent = false, applied = false;
    int i;
    CommonLibF4PlayerHookArgs args;
    const ProxyPlayerRecord* remote;
    LocalPlayerObserverLiveProviderStats provider_stats;
    assert(f4mp_spawn_service(&svc, "7783"));
    usleep(200000);

    proxy_runtime_init();
    assert(mlcb_open(&a, "127.0.0.1", 7783, "guid-pa", "AliceP", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7783, "guid-pb", "BobP", 0x1234u));

    for (i = 0; i < 80; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) && mlcb_local_player_id(&b)) break;
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);

    lpo_init(mlcb_local_player_id(&a));
    lpo_live_provider_install(mlcb_local_player_id(&a));

    memset(&args, 0, sizeof(args));
    args.event.position.x = 13.0f;
    args.event.position.y = 7.0f;
    args.event.rotation.yaw = 30.0f;
    args.event.stance = STANCE_WALK;
    assert(lpo_live_provider_submit(&args));

    for (i = 0; i < 80; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        remote = rpm_get_remote_player(mlcb_local_player_id(&a));
        if (remote && remote->position.x == 13.0f && remote->position.y == 7.0f) break;
        usleep(10000);
    }

    remote = rpm_get_remote_player(mlcb_local_player_id(&a));
    assert(remote != 0);
    assert(remote->position.x == 13.0f);
    assert(remote->position.y == 7.0f);
    assert(remote->rotation.yaw == 30.0f);

    provider_stats = lpo_live_provider_stats();
    assert(provider_stats.submit_count == 1);
    assert(provider_stats.live_accept_count == 1);
    assert(provider_stats.live_reject_count == 0);
    assert(provider_stats.last_state.player_id == mlcb_local_player_id(&a));

    lpo_live_provider_uninstall();
    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
