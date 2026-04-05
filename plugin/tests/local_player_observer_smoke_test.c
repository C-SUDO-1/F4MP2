#include <assert.h>
#include <string.h>
#include "local_player_observer.h"

int main(void) {
    LocalPlayerObservedState s;
    LocalPlayerObservedState out;
    memset(&s, 0, sizeof(s));
    lpo_init(1);
    s.player_id = 1;
    s.valid = true;
    s.position.x = 10.0f;
    s.rotation.yaw = 90.0f;
    s.stance = STANCE_RUN;
    lpo_set_synthetic_state(&s);
    assert(lpo_capture(&out));
    assert(out.player_id == 1);
    assert(out.position.x == 10.0f);
    assert(out.rotation.yaw == 90.0f);
    assert(out.stance == STANCE_RUN);
    return 0;
}
