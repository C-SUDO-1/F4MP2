#include "world_trace_provider.h"

void wtp_init(WorldTraceProvider* p) {
    if (!p) return;
    p->trace_fn = 0;
    p->user = 0;
    p->default_clear_when_unset = true;
}

void wtp_set_trace(WorldTraceProvider* p, WorldTraceFn fn, void* user) {
    if (!p) return;
    p->trace_fn = fn;
    p->user = user;
}

bool wtp_trace_allows(const WorldTraceProvider* p, Vec3f origin, Vec3f target) {
    TraceHitResult hit;
    if (!p) return true;
    if (!p->trace_fn) return p->default_clear_when_unset;
    hit.blocked = false;
    hit.hit_fraction = 1.0f;
    if (!p->trace_fn(origin, target, &hit, p->user)) return false;
    return !hit.blocked;
}
