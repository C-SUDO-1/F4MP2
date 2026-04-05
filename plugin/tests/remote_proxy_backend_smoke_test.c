#include <assert.h>
#include <string.h>
#include "remote_proxy_manager.h"
#include "remote_proxy_backend.h"

typedef struct CaptureState {
    int spawn_count;
    int update_count;
    int despawn_count;
    ProxyPlayerSpawnSpec last_spawn;
    ProxyPlayerUpdateSpec last_update;
    PlayerId last_despawn;
} CaptureState;

static bool cap_spawn(void* user, const ProxyPlayerSpawnSpec* spec) {
    CaptureState* s = (CaptureState*)user;
    s->spawn_count++;
    s->last_spawn = *spec;
    return true;
}

static bool cap_update(void* user, const ProxyPlayerUpdateSpec* spec) {
    CaptureState* s = (CaptureState*)user;
    s->update_count++;
    s->last_update = *spec;
    return true;
}

static bool cap_despawn(void* user, PlayerId player_id) {
    CaptureState* s = (CaptureState*)user;
    s->despawn_count++;
    s->last_despawn = player_id;
    return true;
}

int main(void) {
    CaptureState st;
    RemoteProxyBackend b;
    MsgPlayerState msg;

    memset(&st, 0, sizeof(st));
    memset(&b, 0, sizeof(b));
    memset(&msg, 0, sizeof(msg));

    b.user = &st;
    b.spawn_player = cap_spawn;
    b.update_player = cap_update;
    b.despawn_player = cap_despawn;

    rpm_init(1);
    rpb_install_backend(&b);

    msg.player_id = 2;
    msg.position.x = 5.0f;
    msg.rotation.yaw = 45.0f;
    msg.stance = STANCE_RUN;
    assert(rpm_apply_remote_player_state(&msg));
    assert(st.spawn_count == 1);
    assert(st.update_count == 1);
    assert(st.last_spawn.player_id == 2);
    assert(st.last_update.position.x == 5.0f);
    assert(st.last_update.rotation.yaw == 45.0f);

    assert(rpm_remove_remote_player(2));
    assert(st.despawn_count == 1);
    assert(st.last_despawn == 2);

    rpb_reset_default_backend();
    return 0;
}
