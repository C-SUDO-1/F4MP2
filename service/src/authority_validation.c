#include "authority_validation.h"
#include "service_indices.h"
#include "workshop_whitelist.h"
#include "combat_geometry.h"
#include "los_policy.h"

static float sqrf(float v) { return v * v; }
static float distance_sq(Vec3f a, Vec3f b) { return sqrf(a.x-b.x)+sqrf(a.y-b.y)+sqrf(a.z-b.z); }

CombatGeometryConfig av_default_combat_geometry(void) {
    CombatGeometryConfig cfg;
    cfg.max_range = 4096.0f;
    cfg.min_dot = 0.35f;
    return cfg;
}

bool validate_player_connected(const SessionState* s, PlayerId player_id) {
    const PlayerRuntimeState* p = find_player_const(s, player_id);
    return p != NULL && p->connected;
}

bool validate_player_inside_bubble(const SessionState* s, PlayerId player_id) {
    const PlayerRuntimeState* p = find_player_const(s, player_id);
    if (!p || !p->connected) return false;
    return distance_sq(p->replicated_state.position, s->bubble.center) <= (s->bubble.radius * s->bubble.radius);
}

bool validate_build_request(const SessionState* s, const MsgBuildRequest* req) {
    const PlayerRuntimeState* p;
    const SettlementState* settlement;
    if (!s || !req) return false;
    if (!(s->rules & RULE_HOST_AUTH_WORKSHOP)) return false;
    p = find_player_const(s, req->player_id);
    if (!p || !p->connected) return false;
    settlement = find_settlement_const(s, req->settlement_id);
    if (!settlement || !settlement->in_use) return false;
    if (settlement->object_count >= settlement->object_budget) return false;
    if (distance_sq(req->position, settlement->center) > (settlement->allowed_radius * settlement->allowed_radius)) return false;
    if (!workshop_whitelist_contains(req->form_id)) return false;
    if (req->snap_target_net_id != 0) {
        const ObjectRuntimeState* obj = find_object_const(s, req->snap_target_net_id);
        if (!obj || !obj->in_use || obj->settlement_id != req->settlement_id) return false;
    }
    return true;
}

bool validate_fire_intent(const SessionState* s, const MsgFireIntent* req) {
    const PlayerRuntimeState* p;
    float mag2;
    Vec3f dir;
    if (!s || !req) return false;
    if (!(s->rules & RULE_HOST_AUTH_COMBAT)) return false;
    p = find_player_const(s, req->player_id);
    if (!p || !p->connected) return false;
    if (!validate_player_inside_bubble(s, req->player_id)) return false;
    if (req->weapon_form_id == 0) return false;
    mag2 = sqrf(req->direction.x)+sqrf(req->direction.y)+sqrf(req->direction.z);
    if (mag2 < 0.0001f) return false;
    dir = req->direction;
    if (!cg_normalize(&dir)) return false;
    {
        LosPolicy los;
        Vec3f target = {
            req->origin.x + dir.x * 1024.0f,
            req->origin.y + dir.y * 1024.0f,
            req->origin.z + dir.z * 1024.0f
        };
        los_policy_init(&los);
        los_policy_set_provider(&los, &s->trace_provider);
        if (!los_policy_allows(&los, req->origin, target)) return false;
    }
    return true;
}

bool validate_object_move_request(const SessionState* s, const MsgObjectMoveRequest* req) {
    const PlayerRuntimeState* p;
    const ObjectRuntimeState* obj;
    const SettlementState* settlement;
    if (!s || !req) return false;
    if (!(s->rules & RULE_HOST_AUTH_WORKSHOP)) return false;
    p = find_player_const(s, req->player_id);
    if (!p || !p->connected) return false;
    obj = find_object_const(s, req->object_net_id);
    if (!obj || !obj->in_use) return false;
    settlement = find_settlement_const(s, obj->settlement_id);
    if (!settlement || !settlement->in_use) return false;
    if (distance_sq(req->position, settlement->center) > (settlement->allowed_radius * settlement->allowed_radius)) return false;
    return true;
}

bool validate_object_scrap_request(const SessionState* s, const MsgObjectScrapRequest* req) {
    const PlayerRuntimeState* p;
    const ObjectRuntimeState* obj;
    if (!s || !req) return false;
    if (!(s->rules & RULE_HOST_AUTH_WORKSHOP)) return false;
    p = find_player_const(s, req->player_id);
    if (!p || !p->connected) return false;
    obj = find_object_const(s, req->object_net_id);
    if (!obj || !obj->in_use) return false;
    return true;
}
