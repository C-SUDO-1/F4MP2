#include "interest_manager.h"
#include "service_indices.h"

static float sqrf(float v) { return v * v; }
static float distance_sq(Vec3f a, Vec3f b) { return sqrf(a.x-b.x)+sqrf(a.y-b.y)+sqrf(a.z-b.z); }

void interest_rebuild_bubble(SessionState* s, Vec3f center, float radius, const CellId* cells, uint16_t cell_count) {
    uint16_t i;
    if (!s) return;
    s->bubble.center = center;
    s->bubble.radius = radius;
    if (cell_count > 128) cell_count = 128;
    s->bubble.cell_count = cell_count;
    for (i = 0; i < cell_count; ++i) s->bubble.cells[i] = cells[i];
}

bool interest_actor_relevant_to_player(const SessionState* s, PlayerId player_id, const ActorRuntimeState* actor) {
    const PlayerRuntimeState* p;
    if (!s || !actor || !actor->in_use) return false;
    p = find_player_const(s, player_id);
    if (!p || !p->connected) return false;
    return distance_sq(actor->replicated_state.position, p->replicated_state.position) <= (s->bubble.radius * s->bubble.radius);
}

bool interest_object_relevant_to_player(const SessionState* s, PlayerId player_id, const ObjectRuntimeState* object) {
    const PlayerRuntimeState* p;
    if (!s || !object || !object->in_use) return false;
    p = find_player_const(s, player_id);
    if (!p || !p->connected) return false;
    return distance_sq(object->position, p->replicated_state.position) <= (s->bubble.radius * s->bubble.radius);
}
