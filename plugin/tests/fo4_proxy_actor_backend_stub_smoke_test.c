#include <assert.h>
#include <string.h>

#include "fo4_proxy_actor_backend_stub.h"
#include "remote_proxy_manager.h"
#include "messages_player.h"

int main(void) {
    MsgPlayerState ps;
    const Fo4ProxyBackendPlayerState* s;

    rpm_init(2);
    fo4_proxy_backend_stub_install();

    memset(&ps, 0, sizeof(ps));
    ps.player_id = 5;
    ps.position.x = 9.0f;
    ps.position.y = 4.0f;
    ps.rotation.yaw = 77.0f;
    ps.stance = STANCE_WALK;

    assert(rpm_apply_remote_player_state(&ps));
    s = fo4_proxy_backend_stub_get(5);
    assert(s != 0);
    assert(fo4_proxy_backend_stub_spawn_count() == 1);
    assert(fo4_proxy_backend_stub_update_count() == 1);
    assert(s->spawn.position.x == 9.0f);
    assert(s->update.rotation.yaw == 77.0f);

    assert(rpm_remove_remote_player(5));
    assert(fo4_proxy_backend_stub_despawn_count() == 1);
    assert(s->despawned);
    return 0;
}
