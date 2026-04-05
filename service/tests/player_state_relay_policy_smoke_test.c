#include <assert.h>
#include <string.h>
#include "player_state_relay_policy.h"

int main(void) {
    PlayerStateRelayStats s; MsgPlayerState msg;
    psr_init(&s);
    memset(&msg, 0, sizeof(msg));
    msg.state_sequence = 1; msg.observed_tick = 1;
    assert(psr_evaluate(&s, &msg) == PSR_ACCEPT);
    assert(s.accepted_count == 1);

    msg.state_sequence = 0; msg.observed_tick = 2;
    assert(psr_evaluate(&s, &msg) == PSR_ACCEPT);
    assert(s.accepted_count == 2);

    msg.state_sequence = 1; msg.observed_tick = 1;
    assert(psr_evaluate(&s, &msg) == PSR_REJECT);
    assert(s.rejected_stale_count == 1);

    msg.state_sequence = 2; msg.observed_tick = 1;
    assert(psr_evaluate(&s, &msg) == PSR_REJECT);
    assert(s.rejected_regressive_tick_count == 1);

    msg.state_sequence = 3; msg.observed_tick = 3;
    assert(psr_evaluate(&s, &msg) == PSR_ACCEPT);
    assert(s.last_sequence == 3);
    assert(s.last_observed_tick == 3);
    return 0;
}
