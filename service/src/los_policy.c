#include "los_policy.h"

void los_policy_init(LosPolicy* p) {
    if (!p) return;
    p->trace_fn = 0;
    p->user = 0;
    p->default_allow_when_unset = true;
    p->provider = 0;
}

void los_policy_set_trace(LosPolicy* p, LosTraceFn fn, void* user) {
    if (!p) return;
    p->trace_fn = fn;
    p->user = user;
}

void los_policy_set_provider(LosPolicy* p, const WorldTraceProvider* provider) {
    if (!p) return;
    p->provider = provider;
}

bool los_policy_allows(const LosPolicy* p, Vec3f origin, Vec3f target) {
    if (!p) return true;
    if (p->provider) return wtp_trace_allows(p->provider, origin, target);
    if (!p->trace_fn) return p->default_allow_when_unset;
    return p->trace_fn(origin, target, p->user);
}
