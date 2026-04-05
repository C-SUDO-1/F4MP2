#include <assert.h>
#include "los_policy.h"
#include "world_trace_provider.h"

static bool allow_trace(Vec3f origin, Vec3f target, void* user) {
    (void)origin; (void)target; (void)user;
    return true;
}

static bool clear_provider(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user) {
    (void)origin; (void)target; (void)user;
    out_hit->blocked = false;
    out_hit->hit_fraction = 1.0f;
    return true;
}

static bool block_provider(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user) {
    (void)origin; (void)target; (void)user;
    out_hit->blocked = true;
    out_hit->hit_fraction = 0.1f;
    return true;
}

int main(void) {
    LosPolicy p;
    WorldTraceProvider w;
    Vec3f a = {0,0,0}, b = {1,0,0};
    los_policy_init(&p);
    assert(los_policy_allows(&p, a, b));
    los_policy_set_trace(&p, allow_trace, 0);
    assert(los_policy_allows(&p, a, b));
    wtp_init(&w);
    wtp_set_trace(&w, clear_provider, 0);
    los_policy_set_provider(&p, &w);
    assert(los_policy_allows(&p, a, b));
    wtp_set_trace(&w, block_provider, 0);
    assert(!los_policy_allows(&p, a, b));
    return 0;
}
