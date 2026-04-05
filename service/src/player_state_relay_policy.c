#include "player_state_relay_policy.h"
#include <string.h>

void psr_init(PlayerStateRelayStats* s) {
    if (!s) return;
    memset(s, 0, sizeof(*s));
}

PlayerStateRelayDecision psr_evaluate(PlayerStateRelayStats* s, const MsgPlayerState* msg) {
    if (!s || !msg) return PSR_REJECT;

    if (msg->state_sequence != 0) {
        if (s->last_sequence != 0 && msg->state_sequence < s->last_sequence) {
            s->rejected_stale_count++;
            return PSR_REJECT;
        }
        if (msg->state_sequence == s->last_sequence && msg->observed_tick != 0 && s->last_observed_tick != 0 && msg->observed_tick <= s->last_observed_tick) {
            s->rejected_stale_count++;
            return PSR_REJECT;
        }
    }

    if (msg->observed_tick != 0 && s->last_observed_tick != 0 && msg->observed_tick < s->last_observed_tick) {
        s->rejected_regressive_tick_count++;
        return PSR_REJECT;
    }

    s->accepted_count++;
    if (msg->state_sequence != 0) s->last_sequence = msg->state_sequence;
    if (msg->observed_tick != 0) s->last_observed_tick = msg->observed_tick;
    return PSR_ACCEPT;
}
