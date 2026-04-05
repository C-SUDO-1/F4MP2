#pragma once

#include <stdbool.h>
#include "protocol_types.h"

typedef struct TraceHitResult {
    bool blocked;
    float hit_fraction;
} TraceHitResult;

typedef bool (*WorldTraceFn)(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user);

typedef struct WorldTraceProvider {
    WorldTraceFn trace_fn;
    void* user;
    bool default_clear_when_unset;
} WorldTraceProvider;

void wtp_init(WorldTraceProvider* p);
void wtp_set_trace(WorldTraceProvider* p, WorldTraceFn fn, void* user);
bool wtp_trace_allows(const WorldTraceProvider* p, Vec3f origin, Vec3f target);
