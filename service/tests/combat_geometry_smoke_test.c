#include <assert.h>
#include "combat_geometry.h"

int main(void) {
    Vec3f dir = {1,0,0};
    CombatGeometryConfig cfg = {100.0f, 0.5f};
    float score = 0.0f;
    assert(cg_normalize(&dir));
    assert(cg_target_within_cone((Vec3f){0,0,0}, dir, (Vec3f){10,0,0}, &cfg, &score));
    assert(score > 0.0f);
    assert(!cg_target_within_cone((Vec3f){0,0,0}, dir, (Vec3f){0,10,0}, &cfg, &score));
    assert(!cg_target_within_cone((Vec3f){0,0,0}, dir, (Vec3f){200,0,0}, &cfg, &score));
    return 0;
}
