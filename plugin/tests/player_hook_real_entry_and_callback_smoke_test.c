#include <assert.h>
#include <string.h>

#include "player_hook_shim.h"
#include "commonlibf4_player_hook_callback_stub.h"

static unsigned int g_seen = 0;

static bool test_cb(const CommonLibF4PlayerHookArgs* args, void* user) {
    (void)user;
    assert(args != NULL);
    assert(args->event.player_id == 77);
    g_seen++;
    return true;
}

int main(void) {
    CommonLibF4PlayerHookCallbackRegistration reg;
    MsgPlayerState msg;
    PlayerHookShimStats shim;
    CommonLibF4PlayerHookCallbackStubStats stub;

    memset(&reg, 0, sizeof(reg));
    reg.callback = test_cb;

    phs_init();
    clf4_player_hook_callback_stub_clear();
    clf4_player_hook_callback_stub_register(&reg);

    memset(&msg, 0, sizeof(msg));
    msg.player_id = 77;
    msg.position.x = 1.0f;
    msg.rotation.yaw = 90.0f;
    msg.stance = STANCE_RUN;
    assert(phs_ingest_real_player_snapshot(&msg));

    shim = phs_stats();
    stub = clf4_player_hook_callback_stub_stats();
    assert(g_seen == 1u);
    assert(shim.real_entry_count == 1u);
    assert(shim.real_dispatch_attempt_count == 1u);
    assert(shim.real_dispatch_success_count == 1u);
    assert(stub.real_emit_count == 1u);
    assert(stub.real_callback_success_count == 1u);
    return 0;
}
