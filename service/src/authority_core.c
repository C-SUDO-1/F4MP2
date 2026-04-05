#include "authority_core.h"
#include "authority_validation.h"
#include "service_indices.h"
#include "world_state_store.h"
#include "combat_geometry.h"

static ObjectNetId g_next_object_id = 1000;
static ActorNetId g_next_actor_id = 2000;

bool ac_handle_input_state(SessionState* s, const MsgInputState* msg, PlayerId player_id) {
    PlayerRuntimeState* p;
    if (!s || !msg) return false;
    p = find_player(s, player_id);
    if (!p || !p->connected) return false;
    p->last_input_tick = msg->client_tick;
    p->replicated_state.equipped_form_id = msg->equipped_form_id;
    p->profile_snapshot.equipped_form_id = msg->equipped_form_id;
    return true;
}

ActorRuntimeState* ac_spawn_hostile_actor(SessionState* s, uint32_t base_form_id, Vec3f position, Rot3f rotation) {
    ActorRuntimeState* a;
    if (!s) return NULL;
    a = ws_alloc_actor(s, g_next_actor_id++, base_form_id, ACTOR_KIND_CREATURE);
    if (!a) return NULL;
    a->replicated_state.actor_net_id = a->actor_net_id;
    a->replicated_state.position = position;
    a->replicated_state.rotation = rotation;
    a->replicated_state.health_norm = 1.0f;
    a->replicated_state.actor_flags = ACTF_HOSTILE;
    return a;
}

bool ac_handle_fire_intent(SessionState* s, const MsgFireIntent* msg, MsgDamageResult* out_damage, MsgDeathEvent* out_death, bool* out_has_death) {
    size_t i;
    ActorRuntimeState* target = NULL;
    float best_score = 0.0f;
    if (out_has_death) *out_has_death = false;
    if (!validate_fire_intent(s, msg)) return false;
    if (!out_damage) return true;

    {
        CombatGeometryConfig cfg = av_default_combat_geometry();
        Vec3f dir = msg->direction;
        cg_normalize(&dir);
        for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
            if (s->actors[i].in_use && !(s->actors[i].replicated_state.actor_flags & ACTF_DEAD) && (s->actors[i].replicated_state.actor_flags & ACTF_HOSTILE)) {
                float total_score = 0.0f;
                if (!cg_target_within_cone(msg->origin, dir, s->actors[i].replicated_state.position, &cfg, &total_score)) {
                    continue;
                }
                if (!target || total_score > best_score) {
                    target = &s->actors[i];
                    best_score = total_score;
                }
            }
        }
    }
    if (!target || best_score <= -1000.0f) return true;

    out_damage->attacker_id = msg->player_id;
    out_damage->victim_actor_net_id = target->actor_net_id;
    out_damage->damage_amount = 0.25f;
    out_damage->damage_type = DMG_BALLISTIC;
    out_damage->reserved0 = 0;
    out_damage->flags = 0;

    target->replicated_state.health_norm -= 0.25f;
    if (target->replicated_state.health_norm < 0.0f) target->replicated_state.health_norm = 0.0f;
    out_damage->health_after = target->replicated_state.health_norm;

    if (target->replicated_state.health_norm <= 0.0f) {
        target->replicated_state.actor_flags |= ACTF_DEAD;
        out_damage->flags |= DMGF_KILLED;
        if (out_death) {
            out_death->victim_actor_net_id = target->actor_net_id;
            out_death->killer_id = msg->player_id;
            out_death->ragdoll_seed = target->actor_net_id ^ 0xA5A5A5A5u;
        }
        if (out_has_death) *out_has_death = true;
    }
    return true;
}

bool ac_handle_build_request(SessionState* s, const MsgBuildRequest* msg, MsgBuildResult* out_result) {
    ObjectRuntimeState* obj;
    SettlementState* settlement;
    if (!s || !msg || !out_result) return false;
    out_result->request_id = msg->request_id;
    out_result->reserved0 = 0;
    if (!validate_build_request(s, msg)) {
        out_result->accepted = 0;
        out_result->reason = BUILD_DENIED_GENERIC;
        out_result->object_net_id = 0;
        return false;
    }
    obj = ws_alloc_object(s, g_next_object_id++, msg->form_id, msg->settlement_id);
    if (!obj) {
        out_result->accepted = 0;
        out_result->reason = BUILD_BUDGET_EXCEEDED;
        out_result->object_net_id = 0;
        return false;
    }
    obj->position = msg->position;
    obj->rotation = msg->rotation;
    obj->owner_player_id = msg->player_id;
    obj->state_flags = OBJF_PLACED_BY_PLAYER;
    settlement = find_settlement(s, msg->settlement_id);
    if (settlement) settlement->object_count++;
    out_result->accepted = 1;
    out_result->reason = BUILD_OK;
    out_result->object_net_id = obj->object_net_id;
    return true;
}

bool ac_handle_object_move(SessionState* s, const MsgObjectMoveRequest* msg) {
    ObjectRuntimeState* obj;
    if (!validate_object_move_request(s, msg)) return false;
    obj = find_object(s, msg->object_net_id);
    if (!obj) return false;
    obj->position = msg->position;
    obj->rotation = msg->rotation;
    obj->state_flags |= OBJF_MOVED;
    return true;
}

bool ac_handle_object_scrap(SessionState* s, const MsgObjectScrapRequest* msg) {
    ObjectRuntimeState* obj;
    SettlementState* settlement;
    if (!validate_object_scrap_request(s, msg)) return false;
    obj = find_object(s, msg->object_net_id);
    if (!obj) return false;
    settlement = find_settlement(s, obj->settlement_id);
    if (settlement && settlement->object_count > 0) settlement->object_count--;
    ws_remove_object(s, msg->object_net_id);
    return true;
}


void ac_tick(SessionState* s) {
    size_t i;
    if (!s) return;
    for (i = 0; i < F4MP_MAX_ACTORS_PER_SESSION; ++i) {
        if (s->actors[i].in_use && (s->actors[i].replicated_state.actor_flags & ACTF_DEAD)) {
            if (s->actors[i].pending_despawn) {
                ws_remove_actor(s, s->actors[i].actor_net_id);
            } else {
                s->actors[i].pending_despawn = true;
            }
        }
    }
}
