#include <assert.h>
#include <string.h>
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"

int main(void) {
    MsgPlayerState msg;
    const ProxyPlayerRecord* rec;
    proxy_runtime_init();
    rpm_init(1);
    memset(&msg, 0, sizeof(msg));
    msg.player_id = 2;
    msg.position.x = 42.0f;
    msg.rotation.yaw = 180.0f;
    msg.stance = STANCE_CROUCH;
    assert(rpm_apply_remote_player_state(&msg));
    rec = rpm_get_remote_player(2);
    assert(rec != 0);
    assert(rec->position.x == 42.0f);
    assert(rec->rotation.yaw == 180.0f);
    assert(rec->stance == STANCE_CROUCH);
    assert(rpm_remove_remote_player(2));
    assert(rpm_get_remote_player(2) == 0);
    return 0;
}
