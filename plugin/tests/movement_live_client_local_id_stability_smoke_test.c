#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test_service_process.h"

#include "movement_live_client_bridge.h"

int main(void) {
    MovementLiveClientBridge a, b;
    F4mpServiceProcess svc;
    bool sent = false;
    bool applied = false;
    int i;

    assert(mlcb_open(&a, "127.0.0.1", 7798, "guid-stab-a", "AliceStable", 0x1234u));
    assert(mlcb_open(&b, "127.0.0.1", 7798, "guid-stab-b", "BobStable", 0x1234u));

    for (i = 0; i < 8; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        usleep(10000);
    }
    assert(mlcb_local_player_id(&a) == 0);
    assert(mlcb_local_player_id(&b) == 0);
    assert(f4mp_spawn_service(&svc, "7798"));
    usleep(200000);

    for (i = 0; i < 120; ++i) {
        assert(mlcb_tick(&a, &sent, &applied));
        assert(mlcb_tick(&b, &sent, &applied));
        if (mlcb_local_player_id(&a) != 0 && mlcb_local_player_id(&b) != 0) break;
        usleep(10000);
    }

    assert(mlcb_local_player_id(&a) != 0);
    assert(mlcb_local_player_id(&b) != 0);

    mlcb_close(&a);
    mlcb_close(&b);
    f4mp_stop_service(&svc);
    return 0;
}
