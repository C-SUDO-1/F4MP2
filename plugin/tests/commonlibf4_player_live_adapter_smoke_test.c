#include <assert.h>
#include <math.h>
#include <string.h>

#include "commonlibf4_player_live_adapter.h"

int main(void) {
    CommonLibF4PlayerHookArgs args;
    CommonLibF4PlayerLiveSample sample;
    CommonLibF4PlayerLiveAdapterStats stats;

    memset(&args, 0, sizeof(args));
    memset(&sample, 0, sizeof(sample));

    clf4_pla_reset(77);

    args.event.position.x = 10.0f;
    args.event.position.y = 2.5f;
    args.event.rotation.yaw = 90.0f;
    args.event.velocity.z = -1.0f;
    args.event.stance = STANCE_RUN;
    args.event.anim_state = 9;
    args.event.equipped_form_id = 0xDEADBEEFu;

    assert(clf4_pla_normalize(&args, &sample));
    assert(sample.state.valid);
    assert(sample.state.player_id == 77);
    assert(sample.used_default_player_id);
    assert(sample.source_kind == CLF4_PLA_SOURCE_HOOK_CALLBACK);
    assert(sample.observed_tick == 1);
    assert(sample.accepted_sequence == 1);
    assert(sample.state.position.x == 10.0f);
    assert(sample.state.rotation.yaw == 90.0f);
    assert(sample.state.velocity.z == -1.0f);
    assert(sample.state.stance == STANCE_RUN);
    assert(sample.state.equipped_form_id == 0xDEADBEEFu);

    stats = clf4_pla_stats();
    assert(stats.normalize_attempt_count == 1);
    assert(stats.accept_count == 1);
    assert(stats.reject_count == 0);
    assert(stats.has_last_sample);
    assert(stats.last_reject_reason == CLF4_PLA_REJECT_NONE);
    assert(stats.last_sample.observed_tick == 1);

    args.event.player_id = 88;
    args.event.stance = 255;
    assert(!clf4_pla_normalize(&args, &sample));
    stats = clf4_pla_stats();
    assert(stats.normalize_attempt_count == 2);
    assert(stats.accept_count == 1);
    assert(stats.reject_count == 1);
    assert(stats.last_reject_reason == CLF4_PLA_REJECT_INVALID_STANCE);

    args.event.stance = STANCE_WALK;
    args.event.position.x = NAN;
    assert(!clf4_pla_normalize(&args, &sample));
    stats = clf4_pla_stats();
    assert(stats.normalize_attempt_count == 3);
    assert(stats.reject_count == 2);
    assert(stats.last_reject_reason == CLF4_PLA_REJECT_INVALID_POSITION);

    return 0;
}
