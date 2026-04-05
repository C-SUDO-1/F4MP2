#include <assert.h>
#include "world_trace_provider.h"

static bool clear_trace(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user) {
    (void)origin; (void)target; (void)user;
    out_hit->blocked = false;
    out_hit->hit_fraction = 1.0f;
    return true;
}

static bool blocked_trace(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user) {
    (void)origin; (void)target; (void)user;
    out_hit->blocked = true;
    out_hit->hit_fraction = 0.2f;
    return true;
}

int main(void) {
    WorldTraceProvider p;
    Vec3f a = {0,0,0}, b = {10,0,0};
    wtp_init(&p);
    assert(wtp_trace_allows(&p, a, b));
    wtp_set_trace(&p, clear_trace, 0);
    assert(wtp_trace_allows(&p, a, b));
    wtp_set_trace(&p, blocked_trace, 0);
    assert(!wtp_trace_allows(&p, a, b));
    return 0;
}
