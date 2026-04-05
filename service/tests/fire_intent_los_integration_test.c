#include <assert.h>
#include <string.h>
#include "session_manager.h"
#include "authority_core.h"
#include "world_state_store.h"
#include "world_trace_provider.h"
#include "session_rules.h"

static bool blocked_trace(Vec3f origin, Vec3f target, TraceHitResult* out_hit, void* user) {
    (void)origin; (void)target; (void)user;
    out_hit->blocked = true;
    out_hit->hit_fraction = 0.25f;
    return true;
}

int main(void) {
    SessionState* s;
    MsgFireIntent fire;
    MsgDamageResult dmg;
    MsgDeathEvent death;
    bool has_death = false;
    sm_init();
    s = sm_create_session(55, RULE_HOST_AUTH_COMBAT, 20, 1);
    assert(s);
    assert(sm_join_session(s, 1, 1001));
    s->bubble.center.x = 0.0f;
    s->bubble.center.y = 0.0f;
    s->bubble.center.z = 0.0f;
    s->bubble.radius = 5000.0f;
    s->players[0].replicated_state.position.x = 0.0f;
    s->players[0].replicated_state.position.y = 0.0f;
    s->players[0].replicated_state.position.z = 0.0f;
    assert(sm_spawn_default_hostile(s, 0xDEADu, (Vec3f){100.0f,0.0f,0.0f}));
    wtp_set_trace(&s->trace_provider, blocked_trace, 0);

    memset(&fire, 0, sizeof(fire));
    fire.player_id = 1;
    fire.weapon_form_id = 0x100u;
    fire.origin.x = 0.0f; fire.origin.y = 0.0f; fire.origin.z = 0.0f;
    fire.direction.x = 1.0f; fire.direction.y = 0.0f; fire.direction.z = 0.0f;
    assert(!ac_handle_fire_intent(s, &fire, &dmg, &death, &has_death));
    return 0;
}
