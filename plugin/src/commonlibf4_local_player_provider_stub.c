#include "commonlibf4_local_player_provider_stub.h"
#include "local_player_observer_provider.h"
#include <string.h>

typedef struct CommonLibF4LocalPlayerProviderStubState {
    bool available;
    bool has_state;
    PlayerId local_player_id;
    LocalPlayerObservedState state;
} CommonLibF4LocalPlayerProviderStubState;

static CommonLibF4LocalPlayerProviderStubState g_stub;

static bool stub_capture(void* user, LocalPlayerObservedState* out_state) {
    CommonLibF4LocalPlayerProviderStubState* s = (CommonLibF4LocalPlayerProviderStubState*)user;
    if (!s || !out_state || !s->available || !s->has_state) return false;
    *out_state = s->state;
    if (out_state->player_id == 0) out_state->player_id = s->local_player_id;
    out_state->valid = true;
    return true;
}

void clf4_lpo_stub_install(PlayerId local_player_id) {
    LocalPlayerObserverProvider p;
    memset(&g_stub, 0, sizeof(g_stub));
    g_stub.available = true;
    g_stub.local_player_id = local_player_id;
    p.user = &g_stub;
    p.capture = stub_capture;
    lpo_install_provider(&p);
}

void clf4_lpo_stub_uninstall(void) {
    lpo_clear_provider();
    memset(&g_stub, 0, sizeof(g_stub));
}

void clf4_lpo_stub_set_available(bool available) {
    g_stub.available = available;
}

void clf4_lpo_stub_feed(const LocalPlayerObservedState* state) {
    if (!state) return;
    g_stub.state = *state;
    if (g_stub.state.player_id == 0) g_stub.state.player_id = g_stub.local_player_id;
    g_stub.state.valid = true;
    g_stub.has_state = true;
}

bool clf4_lpo_stub_has_state(void) {
    return g_stub.has_state;
}

const LocalPlayerObservedState* clf4_lpo_stub_get_last_state(void) {
    return g_stub.has_state ? &g_stub.state : 0;
}
