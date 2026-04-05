#pragma once

#include <stdbool.h>
#include "protocol_types.h"

typedef struct CombatGeometryConfig {
    float max_range;
    float min_dot;
} CombatGeometryConfig;

float cg_length_sq(Vec3f v);
float cg_dot(Vec3f a, Vec3f b);
bool cg_normalize(Vec3f* v);
bool cg_target_within_cone(Vec3f origin, Vec3f dir_normalized, Vec3f target, const CombatGeometryConfig* cfg, float* out_score);
