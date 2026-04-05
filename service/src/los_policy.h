#pragma once

#include <stdbool.h>
#include "protocol_types.h"
#include "world_trace_provider.h"

typedef bool (*LosTraceFn)(Vec3f origin, Vec3f target, void* user);

typedef struct LosPolicy {
    LosTraceFn trace_fn;
    void* user;
    bool default_allow_when_unset;
    const WorldTraceProvider* provider;
} LosPolicy;

void los_policy_init(LosPolicy* p);
void los_policy_set_trace(LosPolicy* p, LosTraceFn fn, void* user);
void los_policy_set_provider(LosPolicy* p, const WorldTraceProvider* provider);
bool los_policy_allows(const LosPolicy* p, Vec3f origin, Vec3f target);
