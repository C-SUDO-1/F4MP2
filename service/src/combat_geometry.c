#include "combat_geometry.h"
#include <math.h>

float cg_length_sq(Vec3f v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

float cg_dot(Vec3f a, Vec3f b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

bool cg_normalize(Vec3f* v) {
    float len2;
    float inv;
    if (!v) return false;
    len2 = cg_length_sq(*v);
    if (len2 < 0.0001f) return false;
    inv = 1.0f / sqrtf(len2);
    v->x *= inv; v->y *= inv; v->z *= inv;
    return true;
}

bool cg_target_within_cone(Vec3f origin, Vec3f dir_normalized, Vec3f target, const CombatGeometryConfig* cfg, float* out_score) {
    Vec3f to;
    float dist2, dist, dot;
    CombatGeometryConfig local;
    if (!cfg) {
        local.max_range = 4096.0f;
        local.min_dot = 0.35f;
        cfg = &local;
    }
    to.x = target.x - origin.x;
    to.y = target.y - origin.y;
    to.z = target.z - origin.z;
    dist2 = cg_length_sq(to);
    if (dist2 < 0.0001f) return false;
    dist = sqrtf(dist2);
    if (dist > cfg->max_range) return false;
    to.x /= dist; to.y /= dist; to.z /= dist;
    dot = cg_dot(dir_normalized, to);
    if (dot < cfg->min_dot) return false;
    if (out_score) *out_score = dot - (dist / cfg->max_range);
    return true;
}
