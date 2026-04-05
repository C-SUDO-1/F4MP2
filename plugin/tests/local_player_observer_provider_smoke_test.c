#include <assert.h>
#include <string.h>
#include "local_player_observer.h"
#include "local_player_observer_provider.h"

static bool capture_cb(void* user, LocalPlayerObservedState* out_state) {
    LocalPlayerObservedState* src = (LocalPlayerObservedState*)user;
    *out_state = *src;
    return true;
}

int main(void) {
    LocalPlayerObservedState synth, provider_state, out;
    LocalPlayerObserverProvider p;

    memset(&synth, 0, sizeof(synth));
    memset(&provider_state, 0, sizeof(provider_state));
    memset(&out, 0, sizeof(out));

    lpo_init(7);
    synth.valid = true;
    synth.player_id = 7;
    synth.position.x = 1.0f;
    lpo_set_synthetic_state(&synth);

    provider_state.player_id = 7;
    provider_state.position.x = 99.0f;
    provider_state.rotation.yaw = 123.0f;
    provider_state.stance = STANCE_CROUCH;

    p.user = &provider_state;
    p.capture = capture_cb;
    lpo_install_provider(&p);

    assert(lpo_capture(&out));
    assert(out.valid);
    assert(out.position.x == 99.0f);
    assert(out.rotation.yaw == 123.0f);
    assert(out.stance == STANCE_CROUCH);

    lpo_clear_provider();
    memset(&out, 0, sizeof(out));
    assert(lpo_capture(&out));
    assert(out.position.x == 1.0f);
    return 0;
}
